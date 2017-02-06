#if !defined(__PETSCDMMESH_HH)
#define __PETSCDMMESH_HH

#include <petscdmmesh.h>
#include <functional>

using ALE::Obj;

PetscErrorCode DMMeshView_Sieve(const ALE::Obj<PETSC_MESH_TYPE>& mesh, PetscViewer viewer);

#undef __FUNCT__
#define __FUNCT__ "DMMeshCreateMatrix"
template<typename Mesh, typename Section>
PetscErrorCode  DMMeshCreateMatrix(const Obj<Mesh>& mesh, const Obj<Section>& section, MatType mtype, Mat *J, int bs = -1, bool fillMatrix = false)
{
  const ALE::Obj<typename Mesh::order_type>& order = mesh->getFactory()->getGlobalOrder(mesh, section->getName(), section);
  int            localSize  = order->getLocalSize();
  int            globalSize = order->getGlobalSize();
  PetscBool      isShell, isBlock, isSeqBlock, isMPIBlock, isSymBlock, isSymSeqBlock, isSymMPIBlock, isSymmetric;
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = MatCreate(mesh->comm(), J);CHKERRQ(ierr);
  ierr = MatSetSizes(*J, localSize, localSize, globalSize, globalSize);CHKERRQ(ierr);
  ierr = MatSetType(*J, mtype);CHKERRQ(ierr);
  ierr = MatSetFromOptions(*J);CHKERRQ(ierr);
  ierr = PetscStrcmp(mtype, MATSHELL, &isShell);CHKERRQ(ierr);
  ierr = PetscStrcmp(mtype, MATBAIJ, &isBlock);CHKERRQ(ierr);
  ierr = PetscStrcmp(mtype, MATSEQBAIJ, &isSeqBlock);CHKERRQ(ierr);
  ierr = PetscStrcmp(mtype, MATMPIBAIJ, &isMPIBlock);CHKERRQ(ierr);
  ierr = PetscStrcmp(mtype, MATSBAIJ, &isSymBlock);CHKERRQ(ierr);
  ierr = PetscStrcmp(mtype, MATSEQSBAIJ, &isSymSeqBlock);CHKERRQ(ierr);
  ierr = PetscStrcmp(mtype, MATMPISBAIJ, &isSymMPIBlock);CHKERRQ(ierr);
  // Check for symmetric storage
  isSymmetric = (PetscBool) (isSymBlock || isSymSeqBlock || isSymMPIBlock);
  if (!isShell) {
    PetscInt *dnz, *onz, bsLocal;

    if (bs < 0) {
      if (isBlock || isSeqBlock || isMPIBlock || isSymBlock || isSymSeqBlock || isSymMPIBlock) {
        const typename Section::chart_type& chart = section->getChart();

        for(typename Section::chart_type::const_iterator c_iter = chart.begin(); c_iter != chart.end(); ++c_iter) {
          if (section->getFiberDimension(*c_iter)) {
            bs = section->getFiberDimension(*c_iter);
            break;
          }
        }
      } else {
        bs = 1;
      }
      // Must have same blocksize on all procs (some might have no points)
      bsLocal = bs;
      ierr = MPI_Allreduce(&bsLocal, &bs, 1, MPIU_INT, MPI_MAX, mesh->comm());CHKERRQ(ierr);
    }
    ierr = PetscMalloc2(localSize/bs, PetscInt, &dnz, localSize/bs, PetscInt, &onz);CHKERRQ(ierr);
#ifdef USE_NEW_OVERLAP
    ierr = preallocateOperatorNewOverlap(mesh, bs, section->getAtlas(), order, dnz, onz, isSymmetric, *J, fillMatrix);CHKERRQ(ierr);
#else
    ierr = preallocateOperatorNew(mesh, bs, section->getAtlas(), order, dnz, onz, isSymmetric, *J, fillMatrix);CHKERRQ(ierr);
#endif
    ierr = PetscFree2(dnz, onz);CHKERRQ(ierr);
    if (isSymmetric) {
      ierr = MatSetOption(*J, MAT_IGNORE_LOWER_TRIANGULAR, PETSC_TRUE);CHKERRQ(ierr);
    }
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "DMMeshCreateGlobalScatter"
template<typename Mesh, typename Section>
PetscErrorCode  DMMeshCreateGlobalScatter(const ALE::Obj<Mesh>& m, const ALE::Obj<Section>& s, VecScatter *scatter)
{
  const ALE::Obj<typename Mesh::order_type>& globalOrder = m->getFactory()->getGlobalOrder(m, s->getName(), s);
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = DMMeshCreateGlobalScatter(m, s, globalOrder, false, scatter);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "DMMeshCreateGlobalScatter"
template<typename Mesh, typename Section>
PetscErrorCode  DMMeshCreateGlobalScatter(const ALE::Obj<Mesh>& m, const ALE::Obj<Section>& s, const ALE::Obj<typename Mesh::label_type>& label, VecScatter *scatter)
{
  const ALE::Obj<typename Mesh::order_type>& globalOrder = m->getFactory()->getGlobalOrder(m, s->getName(), s, -1, label);
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = DMMeshCreateGlobalScatter(m, s, globalOrder, false, scatter);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "DMMeshCreateGlobalScatter"
template<typename Mesh, typename Section>
PetscErrorCode  DMMeshCreateGlobalScatter(const ALE::Obj<Mesh>& m, const std::string& name, const typename Section::chart_type& points, const ALE::Obj<Section>& s, VecScatter *scatter)
{
  const ALE::Obj<typename Mesh::order_type>& globalOrder = m->getFactory()->getGlobalOrder(m, name, points, s);
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = DMMeshCreateGlobalScatter(m, s, globalOrder, false, scatter);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "DMMeshCreateGlobalScatter"
template<typename Mesh, typename Section>
PetscErrorCode  DMMeshCreateGlobalScatter(const ALE::Obj<Mesh>& m, const ALE::Obj<Section>& s, const ALE::Obj<typename Mesh::order_type>& globalOrder, bool includeConstraints, VecScatter *scatter)
{
  typedef typename Mesh::real_section_type::index_type index_type;
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = PetscLogEventBegin(DMMesh_GetGlobalScatter,0,0,0,0);CHKERRQ(ierr);
  const typename Mesh::order_type::chart_type& chart = globalOrder->getChart();
  int *localIndices, *globalIndices;
  int  localSize   = globalOrder->getLocalSize();
  int  overlapSize = -1;
  int  localIndx   = 0, globalIndx = 0;
  Vec  globalVec, localVec;
  IS   localIS, globalIS;

  ierr = VecCreate(m->comm(), &globalVec);CHKERRQ(ierr);
  ierr = VecSetSizes(globalVec, localSize, PETSC_DETERMINE);CHKERRQ(ierr);
  ierr = VecSetFromOptions(globalVec);CHKERRQ(ierr);

  if (includeConstraints) {
    overlapSize = s->sizeWithBC();
    ierr = PetscMalloc(overlapSize*sizeof(int), &localIndices);CHKERRQ(ierr);
    ierr = PetscMalloc(overlapSize*sizeof(int), &globalIndices);CHKERRQ(ierr);
  } else {
    overlapSize = s->size();
    ierr = PetscMalloc(overlapSize*sizeof(int), &localIndices);CHKERRQ(ierr);
    ierr = PetscMalloc(overlapSize*sizeof(int), &globalIndices);CHKERRQ(ierr);
  } // if/else

  // Loop over all local points
  for(typename Mesh::order_type::chart_type::const_iterator p_iter = chart.begin(); p_iter != chart.end(); ++p_iter) {
    // Map local indices to global indices
    if (includeConstraints) {
      s->getIndicesRaw(*p_iter, localIndices, &localIndx, 0);
      s->getIndicesRaw(*p_iter, globalOrder, globalIndices, &globalIndx, 0);
    } else {
      s->getIndices(*p_iter, localIndices, &localIndx, 0, true, true);
      s->getIndices(*p_iter, globalOrder, globalIndices, &globalIndx, 0, true, false);
    }
    //numConstraints += s->getConstraintDimension(*p_iter);
  }
  // Local arrays also have constraints, which are not mapped
  if (localIndx  > overlapSize) SETERRQ2(PETSC_COMM_SELF,PETSC_ERR_ARG_SIZ, "Invalid number of local indices %d, should not be greater than %d", localIndx, overlapSize);
  if (globalIndx > overlapSize) SETERRQ2(PETSC_COMM_SELF,PETSC_ERR_ARG_SIZ, "Invalid number of global indices %d, should not be greater than %d", globalIndx, overlapSize);
  if (globalIndx != localIndx)  SETERRQ2(PETSC_COMM_SELF,PETSC_ERR_ARG_SIZ, "Mismatched number of global indices %d, and local indices %d", globalIndx, localIndx);
  if (m->debug()) {
    globalOrder->view("Global Order");
    for(int i = 0; i < globalIndx; ++i) {
      printf("[%d] localIndex[%d]: %d globalIndex[%d]: %d\n", m->commRank(), i, localIndices[i], i, globalIndices[i]);
    }
  }
  ierr = ISCreateGeneral(PETSC_COMM_SELF, localIndx, localIndices,PETSC_OWN_POINTER,  &localIS);CHKERRQ(ierr);
  ierr = ISCreateGeneral(PETSC_COMM_SELF, globalIndx, globalIndices,PETSC_OWN_POINTER, &globalIS);CHKERRQ(ierr);
  // Can remove this when I test it with NULL
#ifdef PETSC_USE_COMPLEX
  ierr = VecCreateSeqWithArray(PETSC_COMM_SELF, 1,s->getStorageSize(), NULL, &localVec);CHKERRQ(ierr);
#else
  ierr = VecCreateSeqWithArray(PETSC_COMM_SELF, 1,s->getStorageSize(), s->restrictSpace(), &localVec);CHKERRQ(ierr);
#endif
  ierr = VecScatterCreate(localVec, localIS, globalVec, globalIS, scatter);CHKERRQ(ierr);
  ierr = ISDestroy(&globalIS);CHKERRQ(ierr);
  ierr = ISDestroy(&localIS);CHKERRQ(ierr);
  ierr = VecDestroy(&localVec);CHKERRQ(ierr);
  ierr = VecDestroy(&globalVec);CHKERRQ(ierr);
  ierr = PetscLogEventEnd(DMMesh_GetGlobalScatter,0,0,0,0);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

template<typename Mesh, typename Section>
void createOperator(const ALE::Obj<Mesh>& mesh, const ALE::Obj<Section>& s, const ALE::Obj<Mesh>& op) {
  typedef ALE::SieveAlg<Mesh> sieve_alg_type;
  typedef ALE::Mesh<PetscInt,PetscScalar> FlexMesh;
  const typename Section::chart_type& chart = s->getChart();

  // Create local operator
  //   We do not decorate arrows yet
  for(typename Section::chart_type::const_iterator p_iter = chart.begin(); p_iter != chart.end(); ++p_iter) {
    const Obj<typename sieve_alg_type::supportArray>& star = sieve_alg_type::star(mesh, *p_iter);

    for(typename sieve_alg_type::supportArray::const_iterator s_iter = star->begin(); s_iter != star->end(); ++s_iter) {
      const Obj<typename sieve_alg_type::coneArray>& closure = sieve_alg_type::closure(mesh, *s_iter);

      for(typename sieve_alg_type::coneArray::const_iterator c_iter = closure->begin(); c_iter != closure->end(); ++c_iter) {
        op->getSieve()->addCone(*c_iter, *p_iter);
      }
    }
  }
  op->view("Local operator");
  // Construct overlap
  Obj<FlexMesh::send_overlap_type> sendOverlap = mesh->getSendOverlap();
  Obj<FlexMesh::recv_overlap_type> recvOverlap = mesh->getRecvOverlap();
  FlexMesh::renumbering_type&      renumbering = mesh->getRenumbering();

  sendOverlap->view("Mesh send overlap");
  recvOverlap->view("Mesh recv overlap");
  // Complete operator
  typedef ALE::DistributionNew<FlexMesh>::cones_type ConeOverlap;

  ALE::Obj<ConeOverlap> overlapCones = ALE::DistributionNew<FlexMesh>::completeCones(op->getSieve(), op->getSieve(), renumbering, sendOverlap, recvOverlap);
  op->view("Completed operator");
  // Update renumbering and overlap
  overlapCones->view("Overlap cones");
  Obj<FlexMesh::send_overlap_type>        opSendOverlap = op->getSendOverlap();
  Obj<FlexMesh::recv_overlap_type>        opRecvOverlap = op->getRecvOverlap();
  FlexMesh::renumbering_type&             opRenumbering = op->getRenumbering();
  const typename ConeOverlap::chart_type& overlapChart  = overlapCones->getChart();
  int                                     p             = renumbering.size();

  opRenumbering = renumbering;
  for(typename ConeOverlap::chart_type::const_iterator p_iter = overlapChart.begin(); p_iter != overlapChart.end(); ++p_iter) {
    if (opRenumbering.find(p_iter->second) == opRenumbering.end()) {
      opRenumbering[p_iter->second] = p++;
    }
  }
  ALE::SetFromMap<FlexMesh::renumbering_type> opGlobalPoints(opRenumbering);

  ALE::OverlapBuilder<>::constructOverlap(opGlobalPoints, opRenumbering, opSendOverlap, opRecvOverlap);
  sendOverlap->view("Operator send overlap");
  recvOverlap->view("Operator recv overlap");
  // Create global order
  Obj<FlexMesh::order_type> globalOrder = new FlexMesh::order_type(op->comm(), op->debug());

  op->getFactory()->constructLocalOrder(globalOrder, opSendOverlap, opGlobalPoints, s);
  op->getFactory()->calculateOffsets(globalOrder);
  op->getFactory()->updateOrder(globalOrder, opGlobalPoints);
  op->getFactory()->completeOrder(globalOrder, opSendOverlap, opRecvOverlap);
  globalOrder->view("Operator global order");
  // Create dnz/onz or CSR
};

template<typename Atlas>
class AdjVisitor {
public:
  typedef typename ALE::Mesh<PetscInt,PetscScalar>::point_type point_type;
protected:
  Atlas&                 atlas;
  ALE::Mesh<PetscInt,PetscScalar>::sieve_type& adjGraph;
  point_type             p;
public:
  AdjVisitor(Atlas& atlas, ALE::Mesh<PetscInt,PetscScalar>::sieve_type& adjGraph) : atlas(atlas), adjGraph(adjGraph) {};
  void visitPoint(const point_type& point) {
    if (atlas.restrictPoint(point)[0].prefix) {
      adjGraph.addCone(point, p);
    }
  };
  template<typename Arrow>
  void visitArrow(const Arrow&) {};
public:
  void setRoot(const point_type& point) {this->p = point;};
};

#undef __FUNCT__
#define __FUNCT__ "createLocalAdjacencyGraph"
template<typename Mesh, typename Atlas>
PetscErrorCode createLocalAdjacencyGraph(const ALE::Obj<Mesh>& mesh, const ALE::Obj<Atlas>& atlas, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type>& adjGraph)
{
  typedef typename ALE::ISieveVisitor::TransitiveClosureVisitor<typename Mesh::sieve_type, AdjVisitor<Atlas> > ClosureVisitor;
  typedef typename ALE::ISieveVisitor::ConeVisitor<typename Mesh::sieve_type, ClosureVisitor>                  ConeVisitor;
  typedef typename ALE::ISieveVisitor::TransitiveClosureVisitor<typename Mesh::sieve_type, ConeVisitor>        StarVisitor;
  AdjVisitor<Atlas> adjV(*atlas, *adjGraph);
  ClosureVisitor    closureV(*mesh->getSieve(), adjV);
  ConeVisitor       coneV(*mesh->getSieve(), closureV);
  StarVisitor       starV(*mesh->getSieve(), coneV);
  /* In general, we need to get FIAT info that attaches dual basis vectors to sieve points */
  const typename Atlas::chart_type& chart = atlas->getChart();

  PetscFunctionBegin;
  starV.setIsCone(false);
  for(typename Atlas::chart_type::const_iterator c_iter = chart.begin(); c_iter != chart.end(); ++c_iter) {
    adjV.setRoot(*c_iter);
    mesh->getSieve()->support(*c_iter, starV);
    closureV.clear();
    starV.clear();
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "createLocalAdjacencyGraphI"
template<typename Mesh, typename Atlas>
PetscErrorCode createLocalAdjacencyGraphI(const ALE::Obj<Mesh>& mesh, const ALE::Obj<Atlas>& atlas, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type>& adjGraph)
{
  typedef typename ALE::ISieveVisitor::TransitiveClosureVisitor<typename Mesh::sieve_type, AdjVisitor<Atlas> > ClosureVisitor;
  typedef typename ALE::ISieveVisitor::ConeVisitor<typename Mesh::sieve_type, ClosureVisitor>                  ConeVisitor;
  typedef typename ALE::ISieveVisitor::TransitiveClosureVisitor<typename Mesh::sieve_type, ConeVisitor>        StarVisitor;
  AdjVisitor<Atlas> adjV(*atlas, *adjGraph);
  ClosureVisitor    closureV(*mesh->getSieve(), adjV);
  ConeVisitor       coneV(*mesh->getSieve(), closureV);
  StarVisitor       starV(*mesh->getSieve(), coneV);
  /* In general, we need to get FIAT info that attaches dual basis vectors to sieve points */
  const typename Atlas::chart_type& chart = atlas->getChart();

  PetscFunctionBegin;
  // Changes for ISieve
  //   1) Add AdjSizeVisitor to set cone sizes
  //   2) Add new symmetrizeSizes() to ISieve to calculate support sizes
  //   3) Allocate adjGraph
  //   4) Change AdjVisitor to stack up cone rather than calling addPoint()
  //   5) Get points and call setCone() each time
  starV.setIsCone(false);
  for(typename Atlas::chart_type::const_iterator c_iter = chart.begin(); c_iter != chart.end(); ++c_iter) {
    adjV.setRoot(*c_iter);
    mesh->getSieve()->support(*c_iter, starV);
    closureV.clear();
    starV.clear();
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "createLocalAdjacencyGraph"
template<typename Atlas>
PetscErrorCode createLocalAdjacencyGraph(const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar> >& mesh, const ALE::Obj<Atlas>& atlas, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type>& adjGraph)
{
  typedef ALE::SieveAlg<ALE::Mesh<PetscInt,PetscScalar> > sieve_alg_type;
  /* In general, we need to get FIAT info that attaches dual basis vectors to sieve points */
  const typename Atlas::chart_type& chart = atlas->getChart();

  PetscFunctionBegin;
  for(typename Atlas::chart_type::const_iterator c_iter = chart.begin(); c_iter != chart.end(); ++c_iter) {
    const Obj<typename sieve_alg_type::supportArray>& star = sieve_alg_type::star(mesh, *c_iter);

    for(typename sieve_alg_type::supportArray::const_iterator s_iter = star->begin(); s_iter != star->end(); ++s_iter) {
      const Obj<typename sieve_alg_type::coneArray>& closure = sieve_alg_type::closure(mesh, *s_iter);

      for(typename sieve_alg_type::coneArray::const_iterator cl_iter = closure->begin(); cl_iter != closure->end(); ++cl_iter) {
        adjGraph->addCone(*cl_iter, *c_iter);
      }
    }
  }
  PetscFunctionReturn(0);
}

template<typename Arrow>
struct SelectSource : public std::unary_function<Arrow, typename Arrow::source_type>
{
  typename Arrow::source_type& operator()(Arrow& x) const {return x.source;}
  const typename Arrow::source_type& operator()(const Arrow& x) const {return x.source;}
};

template<class Pair>
struct Select1st : public std::unary_function<Pair, typename Pair::first_type>
{
  typename Pair::first_type& operator()(Pair& x) const {return x.first;}
  const typename Pair::first_type& operator()(const Pair& x) const {return x.first;}
};

template<typename Mesh, typename Order>
PetscErrorCode globalizeLocalAdjacencyGraph(const ALE::Obj<Mesh>& mesh, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type>& adjGraph, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::send_overlap_type>& sendOverlap, const ALE::Obj<Order>& globalOrder)
{
  typedef ALE::Mesh<PetscInt,PetscScalar> FlexMesh;
  const int debug = mesh->debug();

  PetscFunctionBegin;
  ALE::PointFactory<typename Mesh::point_type>& pointFactory = ALE::PointFactory<FlexMesh::point_type>::singleton(mesh->comm(), mesh->getSieve()->getChart().max(), mesh->debug());
  // Check for points not in sendOverlap
  std::set<typename Mesh::point_type> interiorPoints;
  std::set<typename Mesh::point_type> overlapSources;
  std::set<typename Mesh::sieve_type::arrow_type> overlapArrows;
  const Obj<FlexMesh::sieve_type::traits::capSequence>& columns = adjGraph->cap();

  for(FlexMesh::sieve_type::traits::capSequence::iterator p_iter = columns->begin(); p_iter != columns->end(); ++p_iter) {
    // This optimization does not work because empty points are sent anyway
    //if (!sendOverlap->support(*p_iter)->size() && globalOrder->restrictPoint(*p_iter)[0].index) {
    if (!sendOverlap->support(*p_iter)->size()) {
      interiorPoints.insert(*p_iter);
    } else {
      // If a point p is in the overlap for process i and an adjacent point q is in the overlap for process j then:
      //   Replace (q, p) with (q', p)
      //   Notice I can reverse the arrow because the graph is initially symmetric
      if (debug) {std::cout << "["<<globalOrder->commRank()<<"] Checking overlap point " << *p_iter << " for overlap neighbors" << std::endl;}
      const Obj<typename FlexMesh::sieve_type::supportSequence>&     neighbors = adjGraph->support(*p_iter);
      const typename FlexMesh::sieve_type::supportSequence::iterator nEnd      = neighbors->end();

      for(typename FlexMesh::sieve_type::supportSequence::iterator n_iter = neighbors->begin(); n_iter != nEnd; ++n_iter) {
        if (sendOverlap->support(*n_iter)->size()) {
          if (debug) {std::cout << "["<<globalOrder->commRank()<<"]   Found overlap neighbor " << *n_iter << std::endl;}
          const Obj<typename FlexMesh::send_overlap_type::supportSequence>&     ranks = sendOverlap->support(*p_iter);
          const typename FlexMesh::send_overlap_type::supportSequence::iterator rEnd  = ranks->end();
          bool equal = true;

          for(typename FlexMesh::send_overlap_type::supportSequence::iterator r_iter = ranks->begin(); r_iter != rEnd; ++r_iter) {
            const Obj<typename FlexMesh::send_overlap_type::supportSequence>&     nRanks = sendOverlap->support(*n_iter);
            const typename FlexMesh::send_overlap_type::supportSequence::iterator nrEnd  = nRanks->end();
            bool found = false;

            if (debug) {std::cout << "["<<globalOrder->commRank()<<"]     Checking overlap rank " << *r_iter << std::endl;}
            for(typename FlexMesh::send_overlap_type::supportSequence::iterator nr_iter = nRanks->begin(); nr_iter != nrEnd; ++nr_iter) {
              if (debug) {std::cout << "["<<globalOrder->commRank()<<"]     Checking neighbor overlap rank " << *nr_iter << std::endl;}
              if (*nr_iter == *r_iter) {
                found = true;
                break;
              }
            }
            if (!found) {
              equal = false;
              break;
            }
          }
          if (!equal) {
            if (debug) {std::cout << "["<<globalOrder->commRank()<<"]   Unequal rank sets, replacing arrow " << *n_iter <<" --> "<<*p_iter << std::endl;}
            overlapArrows.insert(typename Mesh::sieve_type::arrow_type(*n_iter, *p_iter));
          } else {
            if (debug) {std::cout << "["<<globalOrder->commRank()<<"]   Equal rank sets, doing nothing for arrow " << *n_iter <<" --> "<<*p_iter << std::endl;}
          }
        }
      }
    }
  }
  // Renumber those points
  pointFactory.clear();
  pointFactory.setMax(mesh->getSieve()->getChart().max());
  pointFactory.renumberPoints(interiorPoints.begin(), interiorPoints.end());
  //pointFactory.renumberPoints(overlapArrows.begin(), overlapArrows.end(), SelectSource<typename Mesh::sieve_type::arrow_type>());
  // They should use a key extractor: overlapSources.insert(overlapArrows.begin(), overlapArrows.end(), SelectSource<typename Mesh::sieve_type::arrow_type>());
  for(typename std::set<typename Mesh::sieve_type::arrow_type>::const_iterator a_iter = overlapArrows.begin(); a_iter != overlapArrows.end(); ++a_iter) {
    overlapSources.insert(a_iter->source);
  }
  pointFactory.renumberPoints(overlapSources.begin(), overlapSources.end());
  typename ALE::PointFactory<typename Mesh::point_type>::renumbering_type& renumbering    = pointFactory.getRenumbering();
  typename ALE::PointFactory<typename Mesh::point_type>::renumbering_type& invRenumbering = pointFactory.getInvRenumbering();
  // Replace points in local adjacency graph
  const Obj<FlexMesh::sieve_type::traits::baseSequence>& base    = adjGraph->base();
  ALE::Obj<std::vector<typename Mesh::point_type> >       newCone = new std::vector<typename Mesh::point_type>();

  for(FlexMesh::sieve_type::traits::baseSequence::iterator b_iter = base->begin(); b_iter != base->end(); ++b_iter) {
    const ALE::Obj<FlexMesh::sieve_type::coneSequence>& cone    = adjGraph->cone(*b_iter);
    const FlexMesh::sieve_type::coneSequence::iterator  cEnd    = cone->end();
    bool                                                 replace = false;

    for(FlexMesh::sieve_type::coneSequence::iterator c_iter = cone->begin(); c_iter != cEnd; ++c_iter) {
      if (interiorPoints.find(*c_iter) != interiorPoints.end()) {
        newCone->push_back(invRenumbering[*c_iter]);
        replace = true;
      } else {
        newCone->push_back(*c_iter);
      }
    }
    if (interiorPoints.find(*b_iter) != interiorPoints.end()) {
      adjGraph->clearCone(*b_iter);
      adjGraph->setCone(newCone, invRenumbering[*b_iter]);
      if (debug) {std::cout << "["<<globalOrder->commRank()<<"]: Replacing cone for " << *b_iter << "("<<invRenumbering[*b_iter]<<") with" << std::endl;}
    } else if (replace) {
      adjGraph->clearCone(*b_iter);
      adjGraph->setCone(newCone, *b_iter);
      if (debug) {std::cout << "["<<globalOrder->commRank()<<"]: Replacing cone for " << *b_iter << " with" << std::endl;}
    }
    if (debug && ((interiorPoints.find(*b_iter) != interiorPoints.end()) || replace)) {
      for(typename std::vector<typename Mesh::point_type>::const_iterator c_iter = newCone->begin(); c_iter != newCone->end(); ++c_iter) {
        std::cout << "["<<globalOrder->commRank()<<"]:   point " << *c_iter << std::endl;
      }
    }
    newCone->clear();
  }
  // Replace arrows
  for(typename std::set<typename Mesh::sieve_type::arrow_type>::const_iterator a_iter = overlapArrows.begin(); a_iter != overlapArrows.end(); ++a_iter) {
    if (debug) {std::cout << "["<<globalOrder->commRank()<<"]: Replacing " << a_iter->source<<" --> "<<a_iter->target << " with " << invRenumbering[a_iter->source]<<" --> "<<a_iter->target << std::endl;}
    adjGraph->removeArrow(a_iter->source, a_iter->target);
    adjGraph->addArrow(invRenumbering[a_iter->source], a_iter->target);
  }
  // Add new points into ordering
#if 1
  for(typename ALE::PointFactory<typename Mesh::point_type>::renumbering_type::const_iterator p_iter = renumbering.begin(); p_iter != renumbering.end(); ++p_iter) {
    if (debug) {std::cout << "["<<globalOrder->commRank()<<"]: Updating " << p_iter->first << " to " << globalOrder->restrictPoint(p_iter->second)[0] << std::endl;}
    globalOrder->addPoint(p_iter->first);
    globalOrder->updatePoint(p_iter->first, globalOrder->restrictPoint(p_iter->second));
  }
#else
  globalOrder->reallocatePoint(renumbering.begin(), renumbering.end(), Select1st<typename ALE::PointFactory<typename Mesh::point_type>::renumbering_type::const_iterator::value_type>());
  for(typename ALE::PointFactory<typename Mesh::point_type>::renumbering_type::const_iterator p_iter = renumbering.begin(); p_iter != renumbering.end(); ++p_iter) {
    if (debug) {std::cout << "["<<globalOrder->commRank()<<"]: Updating " << p_iter->first << " to " << globalOrder->restrictPoint(p_iter->second)[0] << std::endl;}
    globalOrder->updatePoint(p_iter->first, globalOrder->restrictPoint(p_iter->second));
  }
#endif
  PetscFunctionReturn(0);
}

template<typename Order>
PetscErrorCode globalizeLocalAdjacencyGraph(const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar> >& mesh, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type>& adjGraph, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::send_overlap_type>& sendOverlap, const ALE::Obj<Order>& globalOrder)
{
  PetscFunctionBegin;
  PetscFunctionReturn(0);
}

template<typename Mesh, typename Order>
PetscErrorCode localizeLocalAdjacencyGraph(const ALE::Obj<Mesh>& mesh, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type>& adjGraph, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::send_overlap_type>& sendOverlap, const ALE::Obj<Order>& globalOrder)
{
  PetscFunctionBegin;
  ALE::PointFactory<typename Mesh::point_type>& pointFactory = ALE::PointFactory<ALE::Mesh<PetscInt,PetscScalar>::point_type>::singleton(mesh->comm(), mesh->getSieve()->getChart().max(), mesh->debug());
  typename ALE::PointFactory<typename Mesh::point_type>::renumbering_type& renumbering = pointFactory.getRenumbering();
  // Replace points in local adjacency graph
  const Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type::traits::baseSequence>& base = adjGraph->base();

  for(ALE::Mesh<PetscInt,PetscScalar>::sieve_type::traits::baseSequence::iterator b_iter = base->begin(); b_iter != base->end(); ++b_iter) {
    // Replace globalized points
    if (renumbering.find(*b_iter) != renumbering.end()) {
      adjGraph->clearCone(renumbering[*b_iter]);
      adjGraph->setCone(adjGraph->cone(*b_iter), renumbering[*b_iter]);
      adjGraph->clearCone(*b_iter);
    }
  }
  PetscFunctionReturn(0);
}

template<typename Order>
PetscErrorCode localizeLocalAdjacencyGraph(const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar> >& mesh, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type>& adjGraph, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::send_overlap_type>& sendOverlap, const ALE::Obj<Order>& globalOrder)
{
  PetscFunctionBegin;
  PetscFunctionReturn(0);
}

template<typename Mesh, typename Order>
PetscErrorCode renumberLocalAdjacencyGraph(const ALE::Obj<Mesh>& mesh, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type>& adjGraph, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::send_overlap_type>& sendOverlap, const ALE::Obj<Order>& globalOrder)
{
  typedef typename ALE::Mesh<PetscInt,PetscScalar> FlexMesh;
  ALE::Obj<std::set<typename Mesh::point_type> > newCone = new std::set<typename Mesh::point_type>();
  const int debug = mesh->debug();

  PetscFunctionBegin;
  ALE::PointFactory<typename Mesh::point_type>& pointFactory = ALE::PointFactory<typename FlexMesh::point_type>::singleton(mesh->comm(), mesh->getSieve()->getChart().max(), debug);
  pointFactory.constructRemoteRenumbering();
  typename ALE::PointFactory<typename Mesh::point_type>::renumbering_type&        renumbering       = pointFactory.getRenumbering();
  typename ALE::PointFactory<typename Mesh::point_type>::remote_renumbering_type& remoteRenumbering = pointFactory.getRemoteRenumbering();
  // Replace points in local adjacency graph
  const Obj<typename FlexMesh::sieve_type::traits::baseSequence>& base = adjGraph->base();

  for(FlexMesh::sieve_type::traits::baseSequence::iterator b_iter = base->begin(); b_iter != base->end(); ++b_iter) {
    // Loop over cone checking for remote points that shadow local points
    const Obj<FlexMesh::sieve_type::traits::coneSequence>&     cone = adjGraph->cone(*b_iter);
    const FlexMesh::sieve_type::traits::coneSequence::iterator cEnd = cone->end();
    bool replace = false;

    if (debug) {std::cout <<"["<<adjGraph->commRank()<<"]: Checking base point " << *b_iter << std::endl;}
    for(FlexMesh::sieve_type::traits::coneSequence::iterator c_iter = cone->begin(); c_iter != cEnd; ++c_iter) {
      bool useOldPoint = true;

      if (debug) {std::cout <<"["<<adjGraph->commRank()<<"]:   cone point " << *c_iter;}
      if (!globalOrder->isLocal(*c_iter)) {
        if (debug) {std::cout << " is nonlocal" << std::endl;}
        for(int p = 0; p < adjGraph->commSize(); ++p) {
          if (p == adjGraph->commRank()) continue;
          if (remoteRenumbering[p].find(*c_iter) != remoteRenumbering[p].end()) {
            if (debug) {std::cout <<"["<<adjGraph->commRank()<<"]:   found " << *c_iter << " on process " << p << " as point " << remoteRenumbering[p][*c_iter];}
            const Obj<FlexMesh::send_overlap_type::traits::coneSequence>& localPoint = sendOverlap->cone(p, remoteRenumbering[p][*c_iter]);

            if (localPoint->size()) {
              if (debug) {std::cout << " with local match " << *localPoint->begin() << std::endl;}
              newCone->insert(*localPoint->begin());
              replace     = true;
              useOldPoint = false;
              break;
            } else {
              if (debug) {std::cout << " but not in sendOverlap" << std::endl;}
            }
          }
        }
      } else {
        if (debug) {std::cout << " is local" << std::endl;}
        if (renumbering.find(*c_iter) != renumbering.end()) {
          if (debug) {std::cout <<"["<<adjGraph->commRank()<<"]:   found " << *c_iter << " locally as point " << renumbering[*c_iter];}
          newCone->insert(renumbering[*c_iter]);
          replace     = true;
          useOldPoint = false;
        }
      }
      if (useOldPoint) {
        newCone->insert(*c_iter);
      }
    }
    if (replace) {
      if (debug > 1) {
        std::cout <<"["<<adjGraph->commRank()<<"]: Replacing cone for " << *b_iter << " due to shadowed points from" << std::endl;
        const Obj<FlexMesh::sieve_type::traits::coneSequence>&     cone = adjGraph->cone(*b_iter);
        const FlexMesh::sieve_type::traits::coneSequence::iterator cEnd = cone->end();
        for(typename FlexMesh::sieve_type::traits::coneSequence::iterator c_iter = cone->begin(); c_iter != cEnd; ++c_iter) {
          std::cout <<"["<<adjGraph->commRank()<<"]:   point " << *c_iter << std::endl;
        }
        std::cout <<"["<<adjGraph->commRank()<<"]: to" << std::endl;
        for(typename std::set<typename Mesh::point_type>::const_iterator c_iter = newCone->begin(); c_iter != newCone->end(); ++c_iter) {
          std::cout <<"["<<adjGraph->commRank()<<"]:   point " << *c_iter << std::endl;
        }
      }
      adjGraph->setCone(newCone, *b_iter);
      newCone->clear();
    }
  }
  PetscFunctionReturn(0);
}

template<typename Order>
PetscErrorCode renumberLocalAdjacencyGraph(const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar> >& mesh, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type>& adjGraph, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::send_overlap_type>& sendOverlap, const ALE::Obj<Order>& globalOrder)
{
  PetscFunctionBegin;
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "preallocateOperator"
/* Problem:
     We want to allocate an operator. This means we want to know the ordering of all unknowns in the sparsity pattern.
     The preexisting overlap will not contain information about all unknowns (columns) in the operator.

   Solution:
     Construct the local sparsity pattern, using globally consistent names for new interior points. Cone complete this
     pattern, which gives an augmented overlap structure. Insert offsets for the new, global point ids in the existing
     order, and then complete the order. This argues for a recreation of the order, rather than use of an existing
     order.

   Problem:
     Figure out sparsity pattern of the operator, when we have already locally numbered all points. The overlap can
     establish common names for shared points, but not for interior points.

   Solution:
     Create a shared resource that bestows globally consistent names.
*/
template<typename Mesh, typename Atlas>
PetscErrorCode preallocateOperator(const ALE::Obj<Mesh>& mesh, const int bs, const ALE::Obj<Atlas>& atlas, const ALE::Obj<typename Mesh::order_type>& globalOrder, PetscInt dnz[], PetscInt onz[], Mat A)
{
  MPI_Comm                              comm      = mesh->comm();
  const int                             rank      = mesh->commRank();
  const int                             debug     = mesh->debug();
  const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar> >            adjBundle = new ALE::Mesh<PetscInt,PetscScalar>(comm, debug);
  const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type> adjGraph  = new ALE::Mesh<PetscInt,PetscScalar>::sieve_type(comm, debug);
  PetscInt       numLocalRows, firstRow;
  PetscErrorCode ierr;

  PetscFunctionBegin;
  adjBundle->setSieve(adjGraph);
  numLocalRows = globalOrder->getLocalSize();
  firstRow     = globalOrder->getGlobalOffsets()[rank];
  ierr = createLocalAdjacencyGraph(mesh, atlas, adjGraph);CHKERRQ(ierr);
  /* Distribute adjacency graph */
  adjBundle->constructOverlap();
  typedef typename Mesh::sieve_type::point_type point_type;
  typedef typename Mesh::send_overlap_type      send_overlap_type;
  typedef typename Mesh::recv_overlap_type      recv_overlap_type;
  typedef typename ALE::Field<send_overlap_type, int, ALE::Section<point_type, point_type> > send_section_type;
  typedef typename ALE::Field<recv_overlap_type, int, ALE::Section<point_type, point_type> > recv_section_type;
  const Obj<send_overlap_type>& vertexSendOverlap = mesh->getSendOverlap();
  const Obj<recv_overlap_type>& vertexRecvOverlap = mesh->getRecvOverlap();
  const Obj<send_overlap_type>  nbrSendOverlap    = new send_overlap_type(comm, debug);
  const Obj<recv_overlap_type>  nbrRecvOverlap    = new recv_overlap_type(comm, debug);
  const Obj<send_section_type>  sendSection       = new send_section_type(comm, debug);
  const Obj<recv_section_type>  recvSection       = new recv_section_type(comm, sendSection->getTag(), debug);

  if (mesh->commSize() > 1) {
    if (debug > 1) adjGraph->view("Original adjacency graph");
    ierr = globalizeLocalAdjacencyGraph(mesh, adjGraph, vertexSendOverlap, globalOrder);
    if (debug > 1) adjGraph->view("Globalized adjacency graph");
    ALE::Distribution<ALE::Mesh<PetscInt,PetscScalar> >::coneCompletion(vertexSendOverlap, vertexRecvOverlap, adjBundle, sendSection, recvSection);
    if (debug > 1) adjGraph->view("Completed adjacency graph");
    ierr = localizeLocalAdjacencyGraph(mesh, adjGraph, vertexSendOverlap, globalOrder);
    if (debug > 1) adjGraph->view("Localized adjacency graph");
    /* Distribute indices for new points */
    ALE::Distribution<ALE::Mesh<PetscInt,PetscScalar> >::updateOverlap(vertexSendOverlap, vertexRecvOverlap, sendSection, recvSection, nbrSendOverlap, nbrRecvOverlap);
    mesh->getFactory()->completeOrder(globalOrder, nbrSendOverlap, nbrRecvOverlap, true);
    if (debug > 1) globalOrder->view("Completed global order");
    ierr = renumberLocalAdjacencyGraph(mesh, adjGraph, vertexSendOverlap, globalOrder);
    if (debug > 1) adjGraph->view("Renumbered adjacency graph");
  }
  /* Read out adjacency graph */
  const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type> graph = adjBundle->getSieve();
  const typename Atlas::chart_type&     chart = atlas->getChart();

  if (debug > 1) graph->view("Adjacency graph");
  ierr = PetscMemzero(dnz, numLocalRows/bs * sizeof(PetscInt));CHKERRQ(ierr);
  ierr = PetscMemzero(onz, numLocalRows/bs * sizeof(PetscInt));CHKERRQ(ierr);
  for(typename Atlas::chart_type::const_iterator c_iter = chart.begin(); c_iter != chart.end(); ++c_iter) {
    const typename Atlas::point_type& point = *c_iter;

    if (globalOrder->isLocal(point)) {
      const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type::traits::coneSequence>& adj   = graph->cone(point);
      const typename Mesh::order_type::value_type&                 rIdx  = globalOrder->restrictPoint(point)[0];
      const int                                                    row   = rIdx.prefix;
      const int                                                    rSize = rIdx.index/bs;

      if ((debug > 1) && ((bs == 1) || rIdx.index%bs)) std::cout << "["<<graph->commRank()<<"]: row "<<row<<": size " << rIdx.index << " bs "<<bs<<std::endl;
      if (rSize == 0) continue;
      for(ALE::Mesh<PetscInt,PetscScalar>::sieve_type::traits::coneSequence::iterator v_iter = adj->begin(); v_iter != adj->end(); ++v_iter) {
        const ALE::Mesh<PetscInt,PetscScalar>::point_type&                 neighbor = *v_iter;
        const typename Mesh::order_type::value_type& cIdx     = globalOrder->restrictPoint(neighbor)[0];
        const int&                                   cSize    = cIdx.index/bs;

        if ((debug > 1) && ((bs == 1) || cIdx.index%bs)) std::cout << "["<<graph->commRank()<<"]:   col "<<cIdx.prefix<<": size " << cIdx.index << " bs "<<bs<<std::endl;
        if (cSize > 0) {
          if (globalOrder->isLocal(neighbor)) {
            for(int r = 0; r < rSize; ++r) {dnz[(row - firstRow)/bs + r] += cSize;}
          } else {
            for(int r = 0; r < rSize; ++r) {onz[(row - firstRow)/bs + r] += cSize;}
          }
        }
      }
    }
  }
  if (debug) {
    for(int r = 0; r < numLocalRows/bs; r++) {
      std::cout << "["<<rank<<"]: dnz["<<r<<"]: " << dnz[r] << " onz["<<r<<"]: " << onz[r] << std::endl;
    }
  }
  ierr = MatSeqAIJSetPreallocation(A, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPIAIJSetPreallocation(A, 0, dnz, 0, onz);CHKERRQ(ierr);
  ierr = MatSeqBAIJSetPreallocation(A, bs, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPIBAIJSetPreallocation(A, bs, 0, dnz, 0, onz);CHKERRQ(ierr);
  // TODO: ierr = MatSetOption(A, MAT_NEW_NONZERO_ALLOCATION_ERR,PETSC_TRUE);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "preallocateOperator"
template<typename Atlas>
PetscErrorCode preallocateOperator(const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar> >& mesh, const int bs, const ALE::Obj<Atlas>& atlas, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::order_type>& globalOrder, PetscInt dnz[], PetscInt onz[], Mat A)
{
  typedef ALE::Mesh<PetscInt,PetscScalar> FlexMesh;
  typedef ALE::SieveAlg<FlexMesh> sieve_alg_type;
  MPI_Comm                             comm      = mesh->comm();
  const ALE::Obj<FlexMesh>             adjBundle = new FlexMesh(comm, mesh->debug());
  const ALE::Obj<FlexMesh::sieve_type> adjGraph  = new FlexMesh::sieve_type(comm, mesh->debug());
  const bool                           bigDebug  = mesh->debug() > 1;
  PetscInt                             numLocalRows, firstRow;
  ///PetscInt      *dnz, *onz;
  PetscErrorCode                        ierr;

  PetscFunctionBegin;
  adjBundle->setSieve(adjGraph);
  numLocalRows = globalOrder->getLocalSize();
  firstRow     = globalOrder->getGlobalOffsets()[mesh->commRank()];
  ///ierr = PetscMalloc2(numLocalRows, PetscInt, &dnz, numLocalRows, PetscInt, &onz);CHKERRQ(ierr);
  /* Create local adjacency graph */
  /*   In general, we need to get FIAT info that attaches dual basis vectors to sieve points */
  const typename Atlas::chart_type& chart = atlas->getChart();

  for(typename Atlas::chart_type::const_iterator c_iter = chart.begin(); c_iter != chart.end(); ++c_iter) {
    const Obj<typename sieve_alg_type::supportArray>& star = sieve_alg_type::star(mesh, *c_iter);

    for(typename sieve_alg_type::supportArray::const_iterator s_iter = star->begin(); s_iter != star->end(); ++s_iter) {
      const Obj<typename sieve_alg_type::coneArray>& closure = sieve_alg_type::closure(mesh, *s_iter);

      for(typename sieve_alg_type::coneArray::const_iterator cl_iter = closure->begin(); cl_iter != closure->end(); ++cl_iter) {
        adjGraph->addCone(*cl_iter, *c_iter);
      }
    }
  }
  if (bigDebug) adjGraph->view("Adjacency graph");
  /* Distribute adjacency graph */
  adjBundle->constructOverlap();
  typedef typename FlexMesh::sieve_type::point_type point_type;
  typedef typename FlexMesh::send_overlap_type send_overlap_type;
  typedef typename FlexMesh::recv_overlap_type recv_overlap_type;
  typedef typename ALE::Field<send_overlap_type, int, ALE::Section<point_type, point_type> > send_section_type;
  typedef typename ALE::Field<recv_overlap_type, int, ALE::Section<point_type, point_type> > recv_section_type;
  const Obj<send_overlap_type>& vertexSendOverlap = mesh->getSendOverlap();
  const Obj<recv_overlap_type>& vertexRecvOverlap = mesh->getRecvOverlap();
  const Obj<send_overlap_type>  nbrSendOverlap    = new send_overlap_type(comm, mesh->debug());
  const Obj<recv_overlap_type>  nbrRecvOverlap    = new recv_overlap_type(comm, mesh->debug());
  const Obj<send_section_type>  sendSection       = new send_section_type(comm, mesh->debug());
  const Obj<recv_section_type>  recvSection       = new recv_section_type(comm, sendSection->getTag(), mesh->debug());

  ALE::Distribution<FlexMesh>::coneCompletion(vertexSendOverlap, vertexRecvOverlap, adjBundle, sendSection, recvSection);
  /* Distribute indices for new points */
  ///ALE::Distribution<FlexMesh>::updateOverlap(sendSection, recvSection, nbrSendOverlap, nbrRecvOverlap);
  ALE::Distribution<FlexMesh>::updateOverlap(vertexSendOverlap, vertexRecvOverlap, sendSection, recvSection, nbrSendOverlap, nbrRecvOverlap);
  mesh->getFactory()->completeOrder(globalOrder, nbrSendOverlap, nbrRecvOverlap, true);
  /* Read out adjacency graph */
  const ALE::Obj<FlexMesh::sieve_type> graph = adjBundle->getSieve();

  ierr = PetscMemzero(dnz, numLocalRows/bs * sizeof(PetscInt));CHKERRQ(ierr);
  ierr = PetscMemzero(onz, numLocalRows/bs * sizeof(PetscInt));CHKERRQ(ierr);
  for(typename Atlas::chart_type::const_iterator c_iter = chart.begin(); c_iter != chart.end(); ++c_iter) {
    const typename Atlas::point_type& point = *c_iter;

    if (globalOrder->isLocal(point)) {
      const ALE::Obj<FlexMesh::sieve_type::traits::coneSequence>& adj   = graph->cone(point);
      const FlexMesh::order_type::value_type&                     rIdx  = globalOrder->restrictPoint(point)[0];
      const int                                                   row   = rIdx.prefix;
      const int                                                   rSize = rIdx.index/bs;

      if (bigDebug && rIdx.index%bs) std::cout << "["<<graph->commRank()<<"]: row "<<row<<": size " << rIdx.index << " bs "<<bs<<std::endl;
      if (rSize == 0) continue;
      for(FlexMesh::sieve_type::traits::coneSequence::iterator v_iter = adj->begin(); v_iter != adj->end(); ++v_iter) {
        const FlexMesh::point_type&             neighbor = *v_iter;
        const FlexMesh::order_type::value_type& cIdx     = globalOrder->restrictPoint(neighbor)[0];
        const int&                              cSize    = cIdx.index/bs;

        if (bigDebug && cIdx.index%bs) std::cout << "["<<graph->commRank()<<"]:   col "<<cIdx.prefix<<": size " << cIdx.index << " bs "<<bs<<std::endl;
        if (cSize > 0) {
          if (globalOrder->isLocal(neighbor)) {
            for(int r = 0; r < rSize; ++r) {dnz[(row - firstRow)/bs + r] += cSize;}
          } else {
            for(int r = 0; r < rSize; ++r) {onz[(row - firstRow)/bs + r] += cSize;}
          }
        }
      }
    }
  }
  if (mesh->debug()) {
    int rank = mesh->commRank();
    for(int r = 0; r < numLocalRows/bs; r++) {
      std::cout << "["<<rank<<"]: dnz["<<r<<"]: " << dnz[r] << " onz["<<r<<"]: " << onz[r] << std::endl;
    }
  }
  ierr = MatSeqAIJSetPreallocation(A, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPIAIJSetPreallocation(A, 0, dnz, 0, onz);CHKERRQ(ierr);
  ierr = MatSeqBAIJSetPreallocation(A, bs, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPIBAIJSetPreallocation(A, bs, 0, dnz, 0, onz);CHKERRQ(ierr);
  ///ierr = PetscFree2(dnz, onz);CHKERRQ(ierr);
  ///TODO: ierr = MatSetOption(A, MAT_NEW_NONZERO_ALLOCATION_ERR,PETSC_TRUE);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "preallocateOperator"
template<typename Atlas>
PetscErrorCode preallocateOperator(const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar> >& mesh, const int bs, const ALE::Obj<Atlas>& rowAtlas, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::order_type>& rowGlobalOrder, const ALE::Obj<Atlas>& colAtlas, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::order_type>& colGlobalOrder, Mat A)
{
  typedef ALE::Mesh<PetscInt,PetscScalar> FlexMesh;
  typedef ALE::SieveAlg<FlexMesh> sieve_alg_type;
  MPI_Comm                             comm      = mesh->comm();
  const ALE::Obj<FlexMesh>             adjBundle = new FlexMesh(comm, mesh->debug());
  const ALE::Obj<FlexMesh::sieve_type> adjGraph  = new FlexMesh::sieve_type(comm, mesh->debug());
  PetscInt       numLocalRows, firstRow;
  PetscInt      *dnz, *onz;
  PetscErrorCode ierr;

  PetscFunctionBegin;
  adjBundle->setSieve(adjGraph);
  numLocalRows = rowGlobalOrder->getLocalSize();
  firstRow     = rowGlobalOrder->getGlobalOffsets()[mesh->commRank()];
  ierr = PetscMalloc2(numLocalRows, PetscInt, &dnz, numLocalRows, PetscInt, &onz);CHKERRQ(ierr);
  /* Create local adjacency graph */
  /*   In general, we need to get FIAT info that attaches dual basis vectors to sieve points */
  const typename Atlas::chart_type& chart = rowAtlas->getChart();

  for(typename Atlas::chart_type::const_iterator c_iter = chart.begin(); c_iter != chart.end(); ++c_iter) {
    const Obj<typename sieve_alg_type::supportArray>& star = sieve_alg_type::star(mesh, *c_iter);

    for(typename sieve_alg_type::supportArray::const_iterator s_iter = star->begin(); s_iter != star->end(); ++s_iter) {
      const Obj<typename sieve_alg_type::coneArray>& closure = sieve_alg_type::closure(mesh, *s_iter);

      for(typename sieve_alg_type::coneArray::const_iterator cl_iter = closure->begin(); cl_iter != closure->end(); ++cl_iter) {
        adjGraph->addCone(*cl_iter, *c_iter);
      }
    }
  }
  /* Distribute adjacency graph */
  adjBundle->constructOverlap();
  typedef typename FlexMesh::sieve_type::point_type point_type;
  typedef typename FlexMesh::send_overlap_type send_overlap_type;
  typedef typename FlexMesh::recv_overlap_type recv_overlap_type;
  typedef typename ALE::Field<send_overlap_type, int, ALE::Section<point_type, point_type> > send_section_type;
  typedef typename ALE::Field<recv_overlap_type, int, ALE::Section<point_type, point_type> > recv_section_type;
  const Obj<send_overlap_type>& vertexSendOverlap = mesh->getSendOverlap();
  const Obj<recv_overlap_type>& vertexRecvOverlap = mesh->getRecvOverlap();
  const Obj<send_overlap_type>  nbrSendOverlap    = new send_overlap_type(comm, mesh->debug());
  const Obj<recv_overlap_type>  nbrRecvOverlap    = new recv_overlap_type(comm, mesh->debug());
  const Obj<send_section_type>  sendSection       = new send_section_type(comm, mesh->debug());
  const Obj<recv_section_type>  recvSection       = new recv_section_type(comm, sendSection->getTag(), mesh->debug());

  ALE::Distribution<FlexMesh>::coneCompletion(vertexSendOverlap, vertexRecvOverlap, adjBundle, sendSection, recvSection);
  /* Distribute indices for new points */
  ALE::Distribution<FlexMesh>::updateOverlap(vertexSendOverlap, vertexRecvOverlap, sendSection, recvSection, nbrSendOverlap, nbrRecvOverlap);
  mesh->getFactory()->completeOrder(rowGlobalOrder, nbrSendOverlap, nbrRecvOverlap, true);
  mesh->getFactory()->completeOrder(colGlobalOrder, nbrSendOverlap, nbrRecvOverlap, true);
  /* Read out adjacency graph */
  const ALE::Obj<FlexMesh::sieve_type> graph = adjBundle->getSieve();

  ierr = PetscMemzero(dnz, numLocalRows/bs * sizeof(PetscInt));CHKERRQ(ierr);
  ierr = PetscMemzero(onz, numLocalRows/bs * sizeof(PetscInt));CHKERRQ(ierr);
  for(typename Atlas::chart_type::const_iterator c_iter = chart.begin(); c_iter != chart.end(); ++c_iter) {
    const typename Atlas::point_type& point = *c_iter;

    if (rowGlobalOrder->isLocal(point)) {
      const ALE::Obj<FlexMesh::sieve_type::traits::coneSequence>& adj   = graph->cone(point);
      const FlexMesh::order_type::value_type&                     rIdx  = rowGlobalOrder->restrictPoint(point)[0];
      const int                                                   row   = rIdx.prefix;
      const int                                                   rSize = rIdx.index/bs;

      //if (rIdx.index%bs) std::cout << "["<<graph->commRank()<<"]: row "<<row<<": size " << rIdx.index << " bs "<<bs<<std::endl;
      if (rSize == 0) continue;
      for(FlexMesh::sieve_type::traits::coneSequence::iterator v_iter = adj->begin(); v_iter != adj->end(); ++v_iter) {
        const FlexMesh::point_type&             neighbor = *v_iter;
        const FlexMesh::order_type::value_type& cIdx     = colGlobalOrder->restrictPoint(neighbor)[0];
        const int&                              cSize    = cIdx.index/bs;

        //if (cIdx.index%bs) std::cout << "["<<graph->commRank()<<"]:   col "<<cIdx.prefix<<": size " << cIdx.index << " bs "<<bs<<std::endl;
        if (cSize > 0) {
          if (colGlobalOrder->isLocal(neighbor)) {
            for(int r = 0; r < rSize; ++r) {dnz[(row - firstRow)/bs + r] += cSize;}
          } else {
            for(int r = 0; r < rSize; ++r) {onz[(row - firstRow)/bs + r] += cSize;}
          }
        }
      }
    }
  }
  if (mesh->debug()) {
    int rank = mesh->commRank();
    for(int r = 0; r < numLocalRows/bs; r++) {
      std::cout << "["<<rank<<"]: dnz["<<r<<"]: " << dnz[r] << " onz["<<r<<"]: " << onz[r] << std::endl;
    }
  }
  ierr = MatSeqAIJSetPreallocation(A, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPIAIJSetPreallocation(A, 0, dnz, 0, onz);CHKERRQ(ierr);
  ierr = MatSeqBAIJSetPreallocation(A, bs, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPIBAIJSetPreallocation(A, bs, 0, dnz, 0, onz);CHKERRQ(ierr);
  ierr = PetscFree2(dnz, onz);CHKERRQ(ierr);
  ierr = MatSetOption(A, MAT_NEW_NONZERO_ALLOCATION_ERR,PETSC_TRUE);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "createAllocationVectors"
template<typename Atlas, typename Order>
PetscErrorCode createAllocationVectors(const int bs, const ALE::Obj<Atlas>& atlas, const ALE::Obj<Order>& globalOrder, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type>& adjGraph, PetscBool isSymmetric, PetscInt dnz[], PetscInt onz[])
{
  typedef ALE::Mesh<PetscInt,PetscScalar> FlexMesh;
  const typename Atlas::chart_type& chart        = atlas->getChart();
  PetscInt                          numLocalRows = globalOrder->getLocalSize();
  PetscInt                          firstRow     = globalOrder->getGlobalOffsets()[atlas->commRank()];
  PetscErrorCode                    ierr;

  PetscFunctionBegin;
  ierr = PetscMemzero(dnz, numLocalRows/bs * sizeof(PetscInt));CHKERRQ(ierr);
  ierr = PetscMemzero(onz, numLocalRows/bs * sizeof(PetscInt));CHKERRQ(ierr);
  for(typename Atlas::chart_type::const_iterator c_iter = chart.begin(); c_iter != chart.end(); ++c_iter) {
    const typename Atlas::point_type& point = *c_iter;

    if (globalOrder->isLocal(point)) {
      const ALE::Obj<typename FlexMesh::sieve_type::coneSequence>& adj   = adjGraph->cone(point);
      const typename Order::value_type& rIdx  = globalOrder->restrictPoint(point)[0];
      const int                         row   = rIdx.prefix;
      const int                         rSize = rIdx.index/bs;

      if ((atlas->debug() > 1) && ((bs == 1) || (rIdx.index%bs == 0))) std::cout << "["<<adjGraph->commRank()<<"]: row "<<row<<": size " << rIdx.index << " bs "<<bs<<std::endl;
      if (rSize == 0) continue;
      for(typename FlexMesh::sieve_type::coneSequence::iterator v_iter = adj->begin(); v_iter != adj->end(); ++v_iter) {
        const typename Atlas::point_type& neighbor = *v_iter;
        const typename Order::value_type& cIdx     = globalOrder->restrictPoint(neighbor)[0];
        const int                         col      = cIdx.prefix>=0 ? cIdx.prefix : -(cIdx.prefix+1);
        const int&                        cSize    = cIdx.index/bs;

        if ((atlas->debug() > 1) && ((bs == 1) || (cIdx.index%bs == 0))) std::cout << "["<<adjGraph->commRank()<<"]:   col "<<col<<": size " << cIdx.index << " bs "<<bs<<std::endl;
        if (cSize > 0) {
          if (isSymmetric && (col < row)) {
            if (atlas->debug() > 1) {std::cout << "["<<adjGraph->commRank()<<"]: Rejecting row "<<row<<" col " << col <<std::endl;}
            continue;
          }
          if (globalOrder->isLocal(neighbor)) {
            for(int r = 0; r < rSize; ++r) {dnz[(row - firstRow)/bs + r] += cSize;}
          } else {
            for(int r = 0; r < rSize; ++r) {onz[(row - firstRow)/bs + r] += cSize;}
          }
        }
      }
    }
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "fillMatrixWithZero"
template<typename Atlas, typename Order>
PetscErrorCode fillMatrixWithZero(Mat A, const int bs, const ALE::Obj<Atlas>& atlas, const ALE::Obj<Order>& globalOrder, const ALE::Obj<ALE::Mesh<PetscInt,PetscScalar>::sieve_type>& adjGraph, PetscBool isSymmetric, PetscInt dnz[], PetscInt onz[])
{
  typedef ALE::Mesh<PetscInt,PetscScalar> FlexMesh;
  const typename Atlas::chart_type& chart        = atlas->getChart();
  PetscInt                          numLocalRows = globalOrder->getLocalSize();
  PetscInt                          firstRow     = globalOrder->getGlobalOffsets()[atlas->commRank()];
  PetscInt                          maxRowLen    = 0;
  PetscErrorCode                    ierr;

  PetscFunctionBegin;
  for(PetscInt r = 0; r < numLocalRows/bs; ++r) {
    maxRowLen = std::max(maxRowLen, dnz[r] + onz[r]);
  }
  PetscInt    *cols   = new PetscInt[maxRowLen];
  PetscScalar *values = new PetscScalar[maxRowLen];

  ierr = PetscMemzero((void *) values, maxRowLen * sizeof(PetscScalar));CHKERRQ(ierr);
  for(typename Atlas::chart_type::const_iterator c_iter = chart.begin(); c_iter != chart.end(); ++c_iter) {
    const typename Atlas::point_type& point  = *c_iter;
    int                               rowLen = 0;

    if (globalOrder->isLocal(point)) {
      const ALE::Obj<typename FlexMesh::sieve_type::coneSequence>& adj   = adjGraph->cone(point);
      const typename Order::value_type& rIdx  = globalOrder->restrictPoint(point)[0];
      const int                         row   = rIdx.prefix;
      const int                         rSize = rIdx.index/bs;

      if (rSize == 0) continue;
      for(typename FlexMesh::sieve_type::coneSequence::iterator v_iter = adj->begin(); v_iter != adj->end(); ++v_iter) {
        const typename Atlas::point_type& neighbor = *v_iter;
        const typename Order::value_type& cIdx     = globalOrder->restrictPoint(neighbor)[0];
        const int                         col      = cIdx.prefix>=0 ? cIdx.prefix : -(cIdx.prefix+1);
        const int&                        cSize    = cIdx.index/bs;

        if (cSize > 0) {
          if (isSymmetric && (col < row)) {
            continue;
          }
          for(int c = col; c < col+cSize; ++c) {
            cols[rowLen++] = c;
          }
        }
      }
      for(int r = 0; r < rSize; ++r) {
        PetscInt fullRow = row + r;

        if (rowLen != dnz[(row - firstRow)/bs+r]+onz[(row - firstRow)/bs+r]) {
          SETERRQ5(atlas->comm(), PETSC_ERR_ARG_WRONG, "Invalid row length %d, should be dnz[%d]: %d + onz[%d]: %d", rowLen, (row - firstRow)/bs+r, dnz[(row - firstRow)/bs+r], (row - firstRow)/bs+r, onz[(row - firstRow)/bs+r]);
        }
        ierr = MatSetValues(A, 1, &fullRow, rowLen, cols, values, INSERT_VALUES);CHKERRQ(ierr);
      }
    }
  }
  delete [] cols;
  delete [] values;
  ierr = MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "preallocateOperatorNew"
template<typename Mesh, typename Atlas>
PetscErrorCode preallocateOperatorNew(const ALE::Obj<Mesh>& mesh, const int bs, const ALE::Obj<Atlas>& atlas, const ALE::Obj<typename Mesh::order_type>& globalOrder, PetscInt dnz[], PetscInt onz[], PetscBool  isSymmetric, Mat A, bool fillMatrix = false)
{
  typedef ALE::Mesh<PetscInt,PetscScalar> FlexMesh;
  typedef typename Mesh::sieve_type        sieve_type;
  typedef typename Mesh::point_type        point_type;
  typedef typename Mesh::send_overlap_type send_overlap_type;
  typedef typename Mesh::recv_overlap_type recv_overlap_type;
  const ALE::Obj<FlexMesh::sieve_type> adjGraph     = new FlexMesh::sieve_type(mesh->comm(), mesh->debug());
  PetscInt                             numLocalRows = globalOrder->getLocalSize();
  PetscInt                             firstRow     = globalOrder->getGlobalOffsets()[mesh->commRank()];
  const PetscInt                       debug        = mesh->debug()/3;
  PetscErrorCode                       ierr;

  PetscFunctionBegin;
  // Create local adjacency graph
  if (debug) mesh->view("Input Mesh");
  if (debug) globalOrder->view("Initial Global Order");
  ierr = createLocalAdjacencyGraph(mesh, atlas, adjGraph);CHKERRQ(ierr);
  if (debug) adjGraph->view("Adjacency Graph");
  // Complete adjacency graph
  typedef ALE::ConeSection<FlexMesh::sieve_type>              cones_wrapper_type;
  typedef ALE::Section<ALE::Pair<int, point_type>, point_type> cones_type;
  Obj<cones_wrapper_type>       cones          = new cones_wrapper_type(adjGraph);
  Obj<cones_type>               overlapCones   = new cones_type(adjGraph->comm(), adjGraph->debug());
  const Obj<send_overlap_type>& sendOverlap    = mesh->getSendOverlap();
  const Obj<recv_overlap_type>& recvOverlap    = mesh->getRecvOverlap();
  const Obj<send_overlap_type>  nbrSendOverlap = new send_overlap_type(mesh->comm(), mesh->debug());
  const Obj<recv_overlap_type>  nbrRecvOverlap = new recv_overlap_type(mesh->comm(), mesh->debug());

  ALE::Pullback::SimpleCopy::copy(sendOverlap, recvOverlap, cones, overlapCones);
  if (debug) overlapCones->view("Overlap Cones");
  // Now overlapCones has the neighbors for any point in the overlap, in the remote numbering
  // Copy overlaps
  {
    const Obj<typename send_overlap_type::traits::capSequence>      sPoints = sendOverlap->cap();
    const typename send_overlap_type::traits::capSequence::iterator sEnd    = sPoints->end();

    for(typename send_overlap_type::traits::capSequence::iterator p_iter = sPoints->begin(); p_iter != sEnd; ++p_iter) {
      const Obj<typename send_overlap_type::supportSequence>      support = sendOverlap->support(*p_iter);
      const typename send_overlap_type::supportSequence::iterator supEnd  = support->end();

      for(typename send_overlap_type::supportSequence::iterator s_iter = support->begin(); s_iter != supEnd; ++s_iter) {
        nbrSendOverlap->addArrow(*p_iter, *s_iter, s_iter.color());
      }
    }
    const Obj<typename recv_overlap_type::traits::baseSequence>      rPoints = recvOverlap->base();
    const typename recv_overlap_type::traits::baseSequence::iterator rEnd    = rPoints->end();

    for(typename recv_overlap_type::traits::baseSequence::iterator p_iter = rPoints->begin(); p_iter != rEnd; ++p_iter) {
      const Obj<typename recv_overlap_type::coneSequence>      cone = recvOverlap->cone(*p_iter);
      const typename recv_overlap_type::coneSequence::iterator cEnd = cone->end();

      for(typename recv_overlap_type::coneSequence::iterator c_iter = cone->begin(); c_iter != cEnd; ++c_iter) {
        nbrRecvOverlap->addArrow(*c_iter, *p_iter, c_iter.color());
      }
    }
  }
  if (debug) nbrSendOverlap->view("Initial Send Overlap");
  if (debug) nbrRecvOverlap->view("Initial Recv Overlap");
  // Update neighbor send overlap from local adjacency
  typedef typename send_overlap_type::target_type rank_type;
  const Obj<typename send_overlap_type::traits::capSequence>      sPoints = sendOverlap->cap();
  const typename send_overlap_type::traits::capSequence::iterator sEnd    = sPoints->end();

  for(typename send_overlap_type::traits::capSequence::iterator p_iter = sPoints->begin(); p_iter != sEnd; ++p_iter) {
    const point_type&                                           localPoint = *p_iter;
    const Obj<typename send_overlap_type::supportSequence>&     ranks      = sendOverlap->support(localPoint);
    const typename send_overlap_type::supportSequence::iterator rEnd       = ranks->end();

    for(typename send_overlap_type::supportSequence::iterator r_iter = ranks->begin(); r_iter != rEnd; ++r_iter) {
      const Obj<typename FlexMesh::sieve_type::coneSequence>& adj    = adjGraph->cone(localPoint);
      typename FlexMesh::sieve_type::coneSequence::iterator   adjEnd = adj->end();

      for(typename FlexMesh::sieve_type::coneSequence::iterator c_iter = adj->begin(); c_iter != adjEnd; ++c_iter) {
        // Check for interior points
        if (!recvOverlap->coneContains(*c_iter, ALE::IsEqual<rank_type>(*r_iter))) {
          nbrSendOverlap->addArrow(*c_iter, *r_iter, -1);
        }
      }
    }
  }
  if (debug) nbrSendOverlap->view("Modified Send Overlap");
  // Update neighbor recv overlap and local adjacency
  const Obj<typename recv_overlap_type::traits::baseSequence>      rPoints = recvOverlap->base();
  const typename recv_overlap_type::traits::baseSequence::iterator rEnd    = rPoints->end();
  point_type maxPoint = std::max(*std::max_element(adjGraph->cap()->begin(),  adjGraph->cap()->end()),
                                 *std::max_element(adjGraph->base()->begin(), adjGraph->base()->end())) + 1;

  for(typename recv_overlap_type::traits::baseSequence::iterator p_iter = rPoints->begin(); p_iter != rEnd; ++p_iter) {
    const point_type&                                        localPoint = *p_iter;
    const Obj<typename recv_overlap_type::coneSequence>&     ranks      = recvOverlap->cone(localPoint);
    const typename recv_overlap_type::coneSequence::iterator rEnd       = ranks->end();

    for(typename recv_overlap_type::coneSequence::iterator r_iter = ranks->begin(); r_iter != rEnd; ++r_iter) {
      const int                              rank        = *r_iter;
      const point_type&                      remotePoint = r_iter.color();
      const int                              size        = overlapCones->getFiberDimension(typename cones_type::point_type(rank, remotePoint));
      const typename cones_type::value_type *values      = overlapCones->restrictPoint(typename cones_type::point_type(rank, remotePoint));

      for(int i = 0; i < size; ++i) {
        // Check for interior point
        if (!sendOverlap->cone(rank, values[i])->size()) {
          // Check that we have not seen it before
          const Obj<typename recv_overlap_type::supportSequence>& newPoints = nbrRecvOverlap->support(rank, values[i]);
          point_type newPoint;

          if (!newPoints->size()) {
            typename Mesh::order_type::value_type value(-1, 0);

            newPoint = maxPoint++;
            globalOrder->updatePoint(newPoint, &value); // Mark the new point as nonlocal
            nbrRecvOverlap->addArrow(rank, newPoint, values[i]);
          } else {
            newPoint = *newPoints->begin();
          }
          adjGraph->addArrow(newPoint,   localPoint);
          adjGraph->addArrow(localPoint, newPoint);
        } else {
          // Might provide an unknown link for already known point
          const point_type oldPoint = *sendOverlap->cone(rank, values[i])->begin();

          adjGraph->addArrow(oldPoint,   localPoint);
          adjGraph->addArrow(localPoint, oldPoint);
        }
      }
    }
  }
  if (debug) nbrRecvOverlap->view("Modified Recv Overlap");
  if (debug) adjGraph->view("Modified Adjacency Graph");
  mesh->getFactory()->completeOrder(globalOrder, nbrSendOverlap, nbrRecvOverlap);
  if (debug) globalOrder->view("Modified Global Order");
  // Read out adjacency graph
  const typename Atlas::chart_type& chart = atlas->getChart();

  ierr = PetscMemzero(dnz, numLocalRows/bs * sizeof(PetscInt));CHKERRQ(ierr);
  ierr = PetscMemzero(onz, numLocalRows/bs * sizeof(PetscInt));CHKERRQ(ierr);
  for(typename Atlas::chart_type::const_iterator c_iter = chart.begin(); c_iter != chart.end(); ++c_iter) {
    const typename Atlas::point_type& point = *c_iter;

    if (globalOrder->isLocal(point)) {
      const ALE::Obj<typename FlexMesh::sieve_type::traits::coneSequence>& adj   = adjGraph->cone(point);
      const typename Mesh::order_type::value_type&                         rIdx  = globalOrder->restrictPoint(point)[0];
      const int                                                            row   = rIdx.prefix;
      const int                                                            rSize = rIdx.index/bs;

      if ((mesh->debug() > 1) && ((bs == 1) || (rIdx.index%bs == 0))) std::cout << "["<<adjGraph->commRank()<<"]: row "<<row<<": size " << rIdx.index << " bs "<<bs<<std::endl;
      if (rSize == 0) continue;
      for(typename FlexMesh::sieve_type::traits::coneSequence::iterator v_iter = adj->begin(); v_iter != adj->end(); ++v_iter) {
        const typename Mesh::point_type&             neighbor = *v_iter;
        const typename Mesh::order_type::value_type& cIdx     = globalOrder->restrictPoint(neighbor)[0];
        const int                                    col      = cIdx.prefix>=0 ? cIdx.prefix : -(cIdx.prefix+1);
        const int&                                   cSize    = cIdx.index/bs;

        if ((mesh->debug() > 1) && ((bs == 1) || (cIdx.index%bs == 0))) std::cout << "["<<adjGraph->commRank()<<"]:   col "<<col<<": size " << cIdx.index << " bs "<<bs<<std::endl;
        if (cSize > 0) {
          if (isSymmetric && (col < row)) {
            if (mesh->debug() > 1) {std::cout << "["<<adjGraph->commRank()<<"]: Rejecting row "<<row<<" col " << col <<std::endl;}
            continue;
          }
          if (globalOrder->isLocal(neighbor)) {
            for(int r = 0; r < rSize; ++r) {dnz[(row - firstRow)/bs + r] += cSize;}
          } else {
            for(int r = 0; r < rSize; ++r) {onz[(row - firstRow)/bs + r] += cSize;}
          }
        }
      }
    }
  }
  // Set matrix pattern
  ierr = MatSeqAIJSetPreallocation(A, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPIAIJSetPreallocation(A, 0, dnz, 0, onz);CHKERRQ(ierr);
  ierr = MatSeqBAIJSetPreallocation(A, bs, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPIBAIJSetPreallocation(A, bs, 0, dnz, 0, onz);CHKERRQ(ierr);
  ierr = MatSeqSBAIJSetPreallocation(A, bs, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPISBAIJSetPreallocation(A, bs, 0, dnz, 0, onz);CHKERRQ(ierr);
  ierr = MatSetOption(A, MAT_NEW_NONZERO_ALLOCATION_ERR,PETSC_TRUE);CHKERRQ(ierr);
  // Fill matrix with zeros
  if (fillMatrix) {
    int maxRowLen = 0;
    for(int r = 0; r < numLocalRows/bs; ++r) {
      maxRowLen = std::max(maxRowLen, dnz[r] + onz[r]);
    }
    PetscInt    *cols   = new PetscInt[maxRowLen];
    PetscScalar *values = new PetscScalar[maxRowLen];

    ierr = PetscMemzero((void *) values, maxRowLen * sizeof(PetscScalar));CHKERRQ(ierr);
    for(typename Atlas::chart_type::const_iterator c_iter = chart.begin(); c_iter != chart.end(); ++c_iter) {
      const typename Atlas::point_type& point  = *c_iter;
      int                               rowLen = 0;

      if (globalOrder->isLocal(point)) {
        const ALE::Obj<typename FlexMesh::sieve_type::traits::coneSequence>& adj   = adjGraph->cone(point);
        const typename Mesh::order_type::value_type&                         rIdx  = globalOrder->restrictPoint(point)[0];
        const int                                                            row   = rIdx.prefix;
        const int                                                            rSize = rIdx.index/bs;

        if (rSize == 0) continue;
        for(typename FlexMesh::sieve_type::traits::coneSequence::iterator v_iter = adj->begin(); v_iter != adj->end(); ++v_iter) {
          const typename Mesh::point_type&             neighbor = *v_iter;
          const typename Mesh::order_type::value_type& cIdx     = globalOrder->restrictPoint(neighbor)[0];
          const int                                    col      = cIdx.prefix>=0 ? cIdx.prefix : -(cIdx.prefix+1);
          const int&                                   cSize    = cIdx.index/bs;

          if (cSize > 0) {
            if (isSymmetric && (col < row)) {
              continue;
            }
            for(int c = col; c < col+cSize; ++c) {
              cols[rowLen++] = c;
            }
          }
        }
        for(int r = 0; r < rSize; ++r) {
          PetscInt fullRow = row + r;

          if (rowLen != dnz[(row - firstRow)/bs+r]+onz[(row - firstRow)/bs+r]) {
            SETERRQ5(mesh->comm(), PETSC_ERR_ARG_WRONG, "Invalid row length %d, should be dnz[%d]: %d + onz[%d]: %d", rowLen, (row - firstRow)/bs+r, dnz[(row - firstRow)/bs+r], (row - firstRow)/bs+r, onz[(row - firstRow)/bs+r]);
          }
          ierr = MatSetValues(A, 1, &fullRow, rowLen, cols, values, INSERT_VALUES);CHKERRQ(ierr);
        }
      }
    }
    delete [] cols;
    delete [] values;
    ierr = MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
    ierr = MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "preallocateOperatorNewOverlap"
 template<typename Mesh, typename Atlas>
PetscErrorCode preallocateOperatorNewOverlap(const ALE::Obj<Mesh>& mesh, const int bs, const ALE::Obj<Atlas>& atlas, const ALE::Obj<typename Mesh::order_type>& globalOrder, PetscInt dnz[], PetscInt onz[], PetscBool isSymmetric, Mat A, bool fillMatrix = false)
{
  typedef ALE::Mesh<PetscInt,PetscScalar> FlexMesh;
  typedef typename Mesh::sieve_type        sieve_type;
  typedef typename Mesh::point_type        point_type;
  typedef typename Mesh::send_overlap_type send_overlap_type;
  typedef typename Mesh::recv_overlap_type recv_overlap_type;
  const ALE::Obj<FlexMesh::sieve_type> adjGraph = new FlexMesh::sieve_type(mesh->comm(), mesh->debug());
  const PetscInt                       debug    = mesh->debug()/3;
  PetscErrorCode                       ierr;

  PetscFunctionBegin;
  // Create local adjacency graph
  if (debug) mesh->view("Input Mesh");
  if (debug) globalOrder->view("Initial Global Order");
  ierr = createLocalAdjacencyGraph(mesh, atlas, adjGraph);CHKERRQ(ierr);
  if (debug) adjGraph->view("Adjacency Graph");
  // Complete adjacency graph
  typedef ALE::ConeSection<FlexMesh::sieve_type>              cones_wrapper_type;
  typedef ALE::Section<ALE::Pair<int, point_type>, point_type> cones_type;
  Obj<cones_wrapper_type>       cones          = new cones_wrapper_type(adjGraph);
  Obj<cones_type>               overlapCones   = new cones_type(adjGraph->comm(), adjGraph->debug());
  const Obj<send_overlap_type>& sendOverlap    = mesh->getSendOverlap();
  const Obj<recv_overlap_type>& recvOverlap    = mesh->getRecvOverlap();
  const Obj<send_overlap_type>  nbrSendOverlap = new send_overlap_type(mesh->comm(), mesh->debug());
  const Obj<recv_overlap_type>  nbrRecvOverlap = new recv_overlap_type(mesh->comm(), mesh->debug());

  // Now overlapCones will have the neighbors for any point in the overlap, in the remote numbering
  ALE::Pullback::SimpleCopy::copy(sendOverlap, recvOverlap, cones, overlapCones);
  if (debug) overlapCones->view("Overlap Cones");
  // TODO Copy overlaps
  sendOverlap->copy(nbrSendOverlap.ptr());
  recvOverlap->copy(nbrRecvOverlap.ptr());
  if (debug) nbrSendOverlap->view("Initial Send Overlap");
  if (debug) nbrRecvOverlap->view("Initial Recv Overlap");
  // TODO Update neighbor send overlap from local adjacency
  //   For each localPoint in sendOverlap
  //     For each rank receiving this point
  //       For each adjPoint in adjGraph->cone(point)
  //         If recvOverlap does not contain an arrow (rank, adjPoint, *), meaning the point is not interior to the domain
  //           nbrSendOverlap->addArrow(adjPoint, rank, -1)
  const typename send_overlap_type::baseSequence::iterator sBegin = sendOverlap->baseBegin();
  const typename send_overlap_type::baseSequence::iterator sEnd   = sendOverlap->baseEnd();

  for(typename send_overlap_type::baseSequence::iterator r_iter = sBegin; r_iter != sEnd; ++r_iter) {
    const typename send_overlap_type::target_type            rank   = *r_iter;
    const typename send_overlap_type::coneSequence::iterator pBegin = sendOverlap->coneBegin(*r_iter);
    const typename send_overlap_type::coneSequence::iterator pEnd   = sendOverlap->coneEnd(*r_iter);

    for(typename send_overlap_type::coneSequence::iterator p_iter = pBegin; p_iter != pEnd; ++p_iter) {
      const typename send_overlap_type::source_type               localPoint = *p_iter;
      const typename FlexMesh::sieve_type::coneSequence::iterator adjBegin   = adjGraph->cone(localPoint)->begin();
      const typename FlexMesh::sieve_type::coneSequence::iterator adjEnd     = adjGraph->cone(localPoint)->end();

      for(typename FlexMesh::sieve_type::coneSequence::iterator a_iter = adjBegin; a_iter != adjEnd; ++a_iter) {
        const typename FlexMesh::sieve_type::coneSequence::iterator::value_type adjPoint = *a_iter;

        // Deal with duplication at the assembly stage
        nbrSendOverlap->addArrow(adjPoint, rank, -1);
      }
    }
  }
  nbrSendOverlap->assemble();
  nbrSendOverlap->assemblePoints();
  if (debug) nbrSendOverlap->view("Modified Send Overlap");
  //   Let maxPoint be the first point not contained in adjGraph
  point_type maxPoint = std::max(*std::max_element(adjGraph->cap()->begin(),  adjGraph->cap()->end()),
                                 *std::max_element(adjGraph->base()->begin(), adjGraph->base()->end())) + 1;
  // TODO Update neighbor recv overlap and local adjacency
  //   For each point in recvOverlap
  //     For each rank sending this point
  //       For each adjPoint in the overlap cone from adjGraph for this point
  //         If adjPoint is interior, meaning sendOverlap has no arrow (rank, *, adjPoint) CAN THIS EVER HAPPEN???
  //           If nbrRevOverlap has arrow (rank, newPoint, adjPoint)
  //             Let newPoint = maxPoint, increment maxPoint
  //             Add arrows (point, newPoint) and (newPoint, point) to adjGraph
  //           Else
  //             Add arrows (point, newPoint) and (newPoint, point) to adjGraph
  //         Else
  //           Why would we see a new connection for an old point??? Need an example
  //           We have the arrow (rank, oldPoint, adjPoint)
  //           Add arrows (point, oldPoint) and (oldPoint, point) to adjGraph
  const typename recv_overlap_type::capSequence::iterator rBegin = recvOverlap->capBegin();
  const typename recv_overlap_type::capSequence::iterator rEnd   = recvOverlap->capEnd();

  for(typename recv_overlap_type::capSequence::iterator r_iter = rBegin; r_iter != rEnd; ++r_iter) {
    const int                                                   rank   = *r_iter;
    const typename recv_overlap_type::supportSequence::iterator pBegin = recvOverlap->supportBegin(*r_iter);
    const typename recv_overlap_type::supportSequence::iterator pEnd   = recvOverlap->supportEnd(*r_iter);

    for(typename recv_overlap_type::supportSequence::iterator p_iter = pBegin; p_iter != pEnd; ++p_iter) {
      const point_type&                      localPoint  = *p_iter;
      const point_type&                      remotePoint = p_iter.color();
      const int                              size        = overlapCones->getFiberDimension(typename cones_type::point_type(rank, remotePoint));
      const typename cones_type::value_type *values      = overlapCones->restrictPoint(typename cones_type::point_type(rank, remotePoint));

      for(int i = 0; i < size; ++i) {
        const typename recv_overlap_type::supportSequence::iterator newPointsBegin = nbrRecvOverlap->supportBegin(rank, values[i]);
        const int                                                   numNewPoints   = nbrRecvOverlap->getSupportSize(rank, values[i]);
        point_type                                                  newPoint;

        if (!numNewPoints) {
          typename Mesh::order_type::value_type value(-1, 0);

          newPoint = maxPoint++;
          globalOrder->updatePoint(newPoint, &value); // Mark the new point as nonlocal
          nbrRecvOverlap->addArrow(rank, newPoint, values[i]);
        } else {
          newPoint = *newPointsBegin;
        }
        adjGraph->addArrow(newPoint,   localPoint);
        adjGraph->addArrow(localPoint, newPoint);
      }
    }
  }
  nbrRecvOverlap->assemble();
  nbrRecvOverlap->assemblePoints();
  if (debug) nbrRecvOverlap->view("Modified Recv Overlap");
  if (debug) adjGraph->view("Modified Adjacency Graph");
  // Update global order
  mesh->getFactory()->completeOrder(globalOrder, nbrSendOverlap, nbrRecvOverlap);
  if (debug) globalOrder->view("Modified Global Order");
  // Read out adjacency graph
  ierr = createAllocationVectors(bs, atlas, globalOrder, adjGraph, isSymmetric, dnz, onz);
  // Set matrix pattern
  ierr = MatSeqAIJSetPreallocation(A, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPIAIJSetPreallocation(A, 0, dnz, 0, onz);CHKERRQ(ierr);
  ierr = MatSeqBAIJSetPreallocation(A, bs, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPIBAIJSetPreallocation(A, bs, 0, dnz, 0, onz);CHKERRQ(ierr);
  ierr = MatSeqSBAIJSetPreallocation(A, bs, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPISBAIJSetPreallocation(A, bs, 0, dnz, 0, onz);CHKERRQ(ierr);
  ierr = MatSetUp(A);
  ierr = MatSetOption(A, MAT_NEW_NONZERO_ALLOCATION_ERR,PETSC_TRUE);CHKERRQ(ierr);
  // Fill matrix with zeros
  if (fillMatrix) {ierr = fillMatrixWithZero(A, bs, atlas, globalOrder, adjGraph, isSymmetric, dnz, onz);CHKERRQ(ierr);}
  PetscFunctionReturn(0);
}

template<typename Mesh, typename Atlas>
PetscErrorCode preallocateOperatorI(const ALE::Obj<Mesh>& mesh, const int bs, const ALE::Obj<Atlas>& atlas, const ALE::Obj<typename Mesh::order_type>& globalOrder, PetscInt dnz[], PetscInt onz[], PetscBool  isSymmetric, Mat A)
{
  typedef typename Mesh::sieve_type        sieve_type;
  typedef typename Mesh::point_type        point_type;
  typedef typename Mesh::send_overlap_type send_overlap_type;
  typedef typename Mesh::recv_overlap_type recv_overlap_type;
  const ALE::Obj<typename Mesh::sieve_type> adjGraph     = new typename Mesh::sieve_type(mesh->comm(), mesh->debug());
  PetscInt                                  numLocalRows = globalOrder->getLocalSize();
  PetscInt                                  firstRow     = globalOrder->getGlobalOffsets()[mesh->commRank()];
  const PetscInt                            debug        = 0;
  PetscErrorCode                            ierr;

  PetscFunctionBegin;
  // Create local adjacency graph
  if (debug) mesh->view("Input Mesh");
  if (debug) globalOrder->view("Initial Global Order");
  adjGraph->setChart(mesh);
  ierr = createLocalAdjacencyGraphI(mesh, atlas, adjGraph);CHKERRQ(ierr);
  if (debug) adjGraph->view("Adjacency Graph");

  // Will have to reallocate() adjGraph after adding arrows

  // Rewrite read out from adjGraph to use visitors

  // Set matrix pattern
  ierr = MatSeqAIJSetPreallocation(A, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPIAIJSetPreallocation(A, 0, dnz, 0, onz);CHKERRQ(ierr);
  ierr = MatSeqBAIJSetPreallocation(A, bs, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPIBAIJSetPreallocation(A, bs, 0, dnz, 0, onz);CHKERRQ(ierr);
  ierr = MatSeqSBAIJSetPreallocation(A, bs, 0, dnz);CHKERRQ(ierr);
  ierr = MatMPISBAIJSetPreallocation(A, bs, 0, dnz, 0, onz);CHKERRQ(ierr);
  ierr = MatSetOption(A, MAT_NEW_NONZERO_ALLOCATION_ERR,PETSC_TRUE);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "updateOperator"
template<typename Sieve, typename Visitor>
PetscErrorCode updateOperator(Mat A, const Sieve& sieve, Visitor& iV, const PETSC_MESH_TYPE::point_type& e, PetscScalar array[], InsertMode mode)
{
  PetscFunctionBegin;
  ALE::ISieveTraversal<Sieve>::orientedClosure(sieve, e, iV);
  const PetscInt *indices    = iV.getValues();
  const int       numIndices = iV.getSize();
  PetscErrorCode  ierr;

  ierr = PetscLogEventBegin(DMMesh_updateOperator,0,0,0,0);CHKERRQ(ierr);
  if (sieve.debug()) {
    ierr = PetscPrintf(PETSC_COMM_SELF, "[%d]mat for element %d\n", sieve.commRank(), e);CHKERRQ(ierr);
    for(int i = 0; i < numIndices; i++) {
      ierr = PetscPrintf(PETSC_COMM_SELF, "[%d]mat indices[%d] = %d\n", sieve.commRank(), i, indices[i]);CHKERRQ(ierr);
    }
    for(int i = 0; i < numIndices; i++) {
      ierr = PetscPrintf(PETSC_COMM_SELF, "[%d]", sieve.commRank());CHKERRQ(ierr);
      for(int j = 0; j < numIndices; j++) {
#ifdef PETSC_USE_COMPLEX
        ierr = PetscPrintf(PETSC_COMM_SELF, " (%g,%g)", PetscRealPart(array[i*numIndices+j]), PetscImaginaryPart(array[i*numIndices+j]));CHKERRQ(ierr);
#else
        ierr = PetscPrintf(PETSC_COMM_SELF, " %g", array[i*numIndices+j]);CHKERRQ(ierr);
#endif
      }
      ierr = PetscPrintf(PETSC_COMM_SELF, "\n");CHKERRQ(ierr);
    }
  }
  ierr = MatSetValues(A, numIndices, indices, numIndices, indices, array, mode);
  if (ierr) {
    PetscErrorCode ierr2;
    ierr2 = PetscPrintf(PETSC_COMM_SELF, "[%d]ERROR in updateOperator: point %d\n", sieve.commRank(), e);CHKERRQ(ierr2);
    for(int i = 0; i < numIndices; i++) {
      ierr2 = PetscPrintf(PETSC_COMM_SELF, "[%d]mat indices[%d] = %d\n", sieve.commRank(), i, indices[i]);CHKERRQ(ierr2);
    }
    CHKERRQ(ierr);
  }
  ierr = PetscLogEventEnd(DMMesh_updateOperator,0,0,0,0);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "updateOperator"
template<typename Sieve, typename Visitor>
PetscErrorCode updateOperator(Mat A, const Sieve& rowSieve, Visitor& iVr, const PETSC_MESH_TYPE::point_type& rowE, const Sieve& colSieve, Visitor& iVc, const PETSC_MESH_TYPE::point_type& colE, PetscScalar array[], InsertMode mode)
{
  PetscFunctionBegin;
  ALE::ISieveTraversal<Sieve>::orientedClosure(rowSieve, rowE, iVr);
  ALE::ISieveTraversal<Sieve>::orientedClosure(colSieve, colE, iVc);
  const PetscInt *rowIndices    = iVr.getValues();
  const int       numRowIndices = iVr.getSize();
  const PetscInt *colIndices    = iVc.getValues();
  const int       numColIndices = iVc.getSize();
  PetscErrorCode  ierr;

  ierr = PetscLogEventBegin(DMMesh_updateOperator,0,0,0,0);CHKERRQ(ierr);
  if (rowSieve.debug()) {
    ierr = PetscPrintf(PETSC_COMM_SELF, "[%d]mat for element %d,%d\n", rowSieve.commRank(), rowE, colE);CHKERRQ(ierr);
    for(int i = 0; i < numRowIndices; i++) {
      ierr = PetscPrintf(PETSC_COMM_SELF, "[%d]mat row indices[%d] = %d\n", rowSieve.commRank(), i, rowIndices[i]);CHKERRQ(ierr);
    }
    for(int i = 0; i < numColIndices; i++) {
      ierr = PetscPrintf(PETSC_COMM_SELF, "[%d]mat col indices[%d] = %d\n", rowSieve.commRank(), i, colIndices[i]);CHKERRQ(ierr);
    }
    for(int i = 0; i < numRowIndices; i++) {
      ierr = PetscPrintf(PETSC_COMM_SELF, "[%d]", rowSieve.commRank());CHKERRQ(ierr);
      for(int j = 0; j < numColIndices; j++) {
#ifdef PETSC_USE_COMPLEX
        ierr = PetscPrintf(PETSC_COMM_SELF, " (%g,%g)", PetscRealPart(array[i*numColIndices+j]), PetscImaginaryPart(array[i*numColIndices+j]));CHKERRQ(ierr);
#else
        ierr = PetscPrintf(PETSC_COMM_SELF, " %g", array[i*numColIndices+j]);CHKERRQ(ierr);
#endif
      }
      ierr = PetscPrintf(PETSC_COMM_SELF, "\n");CHKERRQ(ierr);
    }
  }
  ierr = MatSetValues(A, numRowIndices, rowIndices, numColIndices, colIndices, array, mode);
  if (ierr) {
    PetscErrorCode ierr2;
    ierr2 = PetscPrintf(PETSC_COMM_SELF, "[%d]ERROR in updateOperator: point %d,%d\n", rowSieve.commRank(), rowE, colE);CHKERRQ(ierr2);
    for(int i = 0; i < numRowIndices; i++) {
      ierr2 = PetscPrintf(PETSC_COMM_SELF, "[%d]mat row indices[%d] = %d\n", rowSieve.commRank(), i, rowIndices[i]);CHKERRQ(ierr2);
    }
    for(int i = 0; i < numColIndices; i++) {
      ierr2 = PetscPrintf(PETSC_COMM_SELF, "[%d]mat col indices[%d] = %d\n", rowSieve.commRank(), i, colIndices[i]);CHKERRQ(ierr2);
    }
    CHKERRQ(ierr);
  }
  ierr = PetscLogEventEnd(DMMesh_updateOperator,0,0,0,0);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#endif // __PETSCDMMESH_HH
