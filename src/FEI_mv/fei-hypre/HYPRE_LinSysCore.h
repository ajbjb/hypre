// *************************************************************************
// This is the HYPRE implementation of LinearSystemCore.
// *************************************************************************

#ifndef _HYPRE_LinSysCore_h_
#define _HYPRE_LinSysCore_h_

#define HYPRE_FEI_Version() "FEI/HYPRE 2.0.1R3"

// *************************************************************************
// system libraries used
// -------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#ifdef NOFEI
#define GlobalID int
class Lookup
{
   int bogus;
};
#endif

// *************************************************************************
// local enumerations and defines
// -------------------------------------------------------------------------

enum HYsolverID {HYPCG,HYGMRES,HYCGSTAB,HYCGSTABL,HYTFQMR,HYBICGS,HYAMG,
                 HYSUPERLU,HYSUPERLUX,HYY12M,HYAMGE};
enum HYpreconID {HYDIAGONAL,HYPILUT,HYPARASAILS,HYBOOMERAMG,HYML,HYDDILUT,
                 HYPOLY,HYDDICT,HYSCHWARZ,HYEUCLID};

#define HYFEI_HIGHMASK      2147483647-255
#define HYFEI_SPECIALMASK              255
#define HYFEI_SLIDEREDUCE1             256
#define HYFEI_SLIDEREDUCE2             512
#define HYFEI_SLIDEREDUCE3            1024
#define HYFEI_PRINTMAT                2048
#define HYFEI_PRINTREDMAT             4096
#define HYFEI_PRINTSOL                8192
#define HYFEI_DDILUT                 16384
#define HYFEI_SCHURREDUCE1           32768
#define HYFEI_SCHURREDUCE2           65536
#define HYFEI_SCHURREDUCE3          131072
#define HYFEI_PRINTFEINFO           262144
#define HYFEI_AMGDEBUG              524288

// *************************************************************************
// class definition
// -------------------------------------------------------------------------

class HYPRE_LinSysCore
#ifndef NOFEI
           : public LinearSystemCore 
#endif
{
 public:
   HYPRE_LinSysCore(MPI_Comm comm);
   virtual ~HYPRE_LinSysCore();

   // ----------------------------------------------------------------------
   // for creating another one, w/o knowing the run-time type of 'this' one.
   // ----------------------------------------------------------------------

#ifndef NOFEI
   LinearSystemCore* clone();
#endif

   // ----------------------------------------------------------------------
   // parameters : for setting generic argc/argv style parameters.
   // ----------------------------------------------------------------------

   int parameters(int numParams, char** params);

   // ----------------------------------------------------------------------
   // new functions in FEI 1.5 and above (not implemented here)
   // ----------------------------------------------------------------------

   int setLookup(Lookup& lookup);

   int setConnectivities(GlobalID elemBlock, int numElements,
           int numNodesPerElem, const GlobalID* elemIDs,
           const int* const* connNodes) ;

   int setStiffnessMatrices(GlobalID elemBlock, int numElems,
           const GlobalID* elemIDs, const double *const *const *stiff,
           int numEqnsPerElem, const int *const * eqnIndices);

   int setLoadVectors(GlobalID elemBlock, int numElems,
           const GlobalID* elemIDs, const double *const * load,
           int numEqnsPerElem, const int *const * eqnIndices);

   int setMultCREqns(int multCRSetID, int numCRs, int numNodesPerCR,
           int** nodeNumbers, int** eqnNumbers, int* fieldIDs,
           int* multiplierEqnNumbers);

   int setPenCREqns(int penCRSetID, int numCRs, int numNodesPerCR,
           int** nodeNumbers, int** eqnNumbers, int* fieldIDs);

   // ----------------------------------------------------------------------
   // setGlobalOffsets : provide info for initial creation of
   //      matrix/vector data, Equation numbers are 1-based, and local sets
   //      of equation numbers are contiguous.
   // ----------------------------------------------------------------------

   int setGlobalOffsets(int len, int* nodeOffsets, int* eqnOffsets,
                        int* blkEqnOffsets);

   // ----------------------------------------------------------------------
   // setMatrixStructure : provide enough info to allocate the matrix --
   //                      i.e., define the structure.
   // ----------------------------------------------------------------------

   int setMatrixStructure(int** ptColIndices, int* ptRowLengths,
           int** blkColIndices, int* blkRowLengths, int* ptRowsPerBlkRow);

   // ----------------------------------------------------------------------
   // resetMatrixAndVector : don't destroy the structure of the matrix,
   //      but set the value 's' throughout the matrix and vectors.
   // ----------------------------------------------------------------------

   int resetMatrixAndVector(double s);

   // ----------------------------------------------------------------------
   // reset matrix and vector individually
   // ----------------------------------------------------------------------

   int resetMatrix(double s);
   int resetRHSVector(double s);

   // ----------------------------------------------------------------------
   // sumIntoSystemMatrix:
   // this is the primary assembly function. The coefficients 'values'
   // are to be accumumlated into (added to any values already in place)
   // global (0-based) equation 'row' of the matrix.
   // ----------------------------------------------------------------------

   int sumIntoSystemMatrix(int numPtRows, const int* ptRows,
           int numPtCols, const int* ptCols, int numBlkRows,
           const int* blkRows, int numBlkCols, const int* blkCols,
           const double* const* values);

   // ----------------------------------------------------------------------
   // sumIntoSystemMatrix:
   // this is the primary assembly function. The coefficients 'values'
   // are to be accumumlated into (added to any values already in place)
   // global (1-based) [old - 0-based] equation 'row' of the matrix.
   // ----------------------------------------------------------------------

   int sumIntoSystemMatrix(int numPtRows, const int* ptRows,
                           int numPtCols, const int* ptCols,
                           const double* const* values);

   // ----------------------------------------------------------------------
   // Point-entry matrix data as for 'sumIntoSystemMatrix', but in this case
   // the data should be "put" into the matrix (i.e., overwrite any coefficients
   // already present) rather than being "summed" into the matrix.
   // ----------------------------------------------------------------------

   int putIntoSystemMatrix(int numPtRows, const int* ptRows, int numPtCols,
                           const int* ptCols, const double* const* values);

   // ----------------------------------------------------------------------
   // Get the length of a row of the matrix.
   // ----------------------------------------------------------------------

   int getMatrixRowLength(int row, int& length);

   // ----------------------------------------------------------------------
   // Obtain the coefficients and indices for a row of the matrix.
   // ----------------------------------------------------------------------

   int getMatrixRow(int row, double* coefs, int* indices, int len, 
                    int& rowLength);

   // ----------------------------------------------------------------------
   // sumIntoRHSVector:
   // this is the rhs vector equivalent to sumIntoSystemMatrix above.
   // ----------------------------------------------------------------------

   int sumIntoRHSVector(int num, const double* values, const int* indices);

   // ----------------------------------------------------------------------
   // For putting coefficients into the rhs vector
   // ----------------------------------------------------------------------

   int putIntoRHSVector(int num, const double* values, const int* indices);

   // ----------------------------------------------------------------------
   // For getting coefficients out of the rhs vector
   // ----------------------------------------------------------------------

   int getFromRHSVector(int num, double* values, const int* indices);

   // ----------------------------------------------------------------------
   // matrixLoadComplete:
   // do any internal synchronization/communication.
   // ----------------------------------------------------------------------

   int matrixLoadComplete();
   
   // ----------------------------------------------------------------------
   // Pass nodal data that probably doesn't mean anything to the FEI
   // implementation, but may mean something to the linear solver. Examples:
   // geometric coordinates, nullspace data, etc.
   // ----------------------------------------------------------------------

   int putNodalFieldData(int fieldID, int fieldSize, int* nodeNumbers,
                         int numNodes, const double* data);

   // ----------------------------------------------------------------------
   // functions for enforcing boundary conditions.
   // ----------------------------------------------------------------------

   int enforceEssentialBC(int* globalEqn,double* alpha,double* gamma,int len);

   int enforceRemoteEssBCs(int numEqns, int* globalEqns, int** colIndices, 
                           int* colIndLen, double** coefs);

   int enforceOtherBC(int* globalEqn, double* alpha, double* beta, 
                      double* gamma, int len);

   // ----------------------------------------------------------------------
   //functions for getting/setting matrix or vector pointers.
   // ----------------------------------------------------------------------

   // ----------------------------------------------------------------------
   // getMatrixPtr:
   // obtain a pointer to the 'A' matrix. This should be considered a
   // constant pointer -- i.e., this class remains responsible for the
   // matrix (e.g., de-allocation upon destruction).
   // ----------------------------------------------------------------------

#ifndef NOFEI
   int getMatrixPtr(Data& data);
#endif

   // ----------------------------------------------------------------------
   // copyInMatrix:
   // replaces the internal matrix with a copy of the input argument, scaled
   // by the coefficient 'scalar'.
   // ----------------------------------------------------------------------

#ifndef NOFEI
   int copyInMatrix(double scalar, const Data& data);
#endif

   // ----------------------------------------------------------------------
   // copyOutMatrix:
   // passes out a copy of the internal matrix, scaled by the coefficient
   // 'scalar'.
   // ----------------------------------------------------------------------

#ifndef NOFEI
   int copyOutMatrix(double scalar, Data& data);
#endif

   // ----------------------------------------------------------------------
   // sumInMatrix:
   // accumulate (sum) a copy of the input argument into the internal
   // matrix, scaling the input by the coefficient 'scalar'.
   // ----------------------------------------------------------------------

#ifndef NOFEI
   int sumInMatrix(double scalar, const Data& data);
#endif 

   // ----------------------------------------------------------------------
   // get/setRHSVectorPtr:
   // the same semantics apply here as for the matrixPtr functions above.
   // ----------------------------------------------------------------------

#ifndef NOFEI
   int getRHSVectorPtr(Data& data);
#endif 

   // ----------------------------------------------------------------------
   // copyInRHSVector/copyOutRHSVector/sumInRHSVector:
   // the same semantics apply here as for the matrix functions above.
   // ----------------------------------------------------------------------

#ifndef NOFEI
   int copyInRHSVector(double scalar, const Data& data);
   int copyOutRHSVector(double scalar, Data& data);
   int sumInRHSVector(double scalar, const Data& data);
#endif 

   // ----------------------------------------------------------------------
   // destroyMatrixData/destroyVectorData:
   // Utility function for destroying the matrix (or vector) in Data
   // ----------------------------------------------------------------------

#ifndef NOFEI
   int destroyMatrixData(Data& data);
   int destroyVectorData(Data& data);
#endif 

   // ----------------------------------------------------------------------
   // functions for managing multiple rhs vectors
   // ----------------------------------------------------------------------

   int setNumRHSVectors(int numRHSs, const int* rhsIDs);

   // ----------------------------------------------------------------------
   // setRHSID:
   // set the 'current' rhs context, assuming multiple rhs vectors.
   // ----------------------------------------------------------------------

   int setRHSID(int rhsID);

   // ----------------------------------------------------------------------
   // putInitialGuess:
   // function for setting (a subset of) the initial-guess
   // solution values (i.e., in the 'x' vector).
   // ----------------------------------------------------------------------

   int putInitialGuess(const int* eqnNumbers, const double* values,int len);

   // ----------------------------------------------------------------------
   // function for getting all of the answers ('x' vector).
   // ----------------------------------------------------------------------

   int getSolution(double* answers, int len);

   // ----------------------------------------------------------------------
   // function for getting the (single) entry at equation number 'eqnNumber'.
   // ----------------------------------------------------------------------

   int getSolnEntry(int eqnNumber, double& answer);

   // ----------------------------------------------------------------------
   // This will be called to request that LinearSystemCore form the residual
   // vector r = b - A*x, and pass the coefficients for r back out in the
   // 'values' list.
   // ----------------------------------------------------------------------

   int formResidual(double* values, int len);

   // ----------------------------------------------------------------------
   // function for launching the linear solver
   // ----------------------------------------------------------------------

   int launchSolver(int& solveStatus, int& iterations);

   // ----------------------------------------------------------------------
   // other functions
   // ----------------------------------------------------------------------

   int  writeSystem(const char *);

   // ----------------------------------------------------------------------
   // old functions before FEI 1.5 (but still needed here)
   // ----------------------------------------------------------------------

   int createMatricesAndVectors(int numGlobalEqns, int firstLocalEqn,
                                int numLocalEqns);

   int allocateMatrix(int** colIndices, int* rowLengths);

   int sumIntoSystemMatrix(int row, int numValues, const double* values,
                            const int* scatterIndices);

   // ----------------------------------------------------------------------
   // HYPRE-specific public functions
   // ----------------------------------------------------------------------

   void   loadConstraintNumbers(int length, int *list);
   void   buildSlideReducedSystem();
   void   buildSlideReducedSystem2();
   double buildSlideReducedSoln();
   double buildSlideReducedSoln2();
   void   buildSchurReducedSystem();
   void   buildSchurReducedSystem2();
   int    HYPRE_Schur_Search(int,int,int*,int*,int,int);
   void   buildSchurReducedRHS();
   double buildSchurReducedSoln();
   void   computeAConjProjection(HYPRE_ParCSRMatrix A_csr, HYPRE_ParVector x, 
                                 HYPRE_ParVector b);
   void   computeMinResProjection(HYPRE_ParCSRMatrix A_csr, HYPRE_ParVector x, 
                                  HYPRE_ParVector b);
   void   addToAConjProjectionSpace(HYPRE_IJVector x, HYPRE_IJVector b);
   void   addToMinResProjectionSpace(HYPRE_IJVector x, HYPRE_IJVector b);
   char  *getVersion();
   void   beginCreateMapFromSoln();
   void   endCreateMapFromSoln();
   void   putIntoMappedMatrix(int row, int numValues, const double* values,
                              const int* scatterIndices);
   void   getFEGridObject(void **object) { (*object) = fegrid; }

 private:        //functions

   // ----------------------------------------------------------------------
   // functions for selecting solver/preconditioner
   // ----------------------------------------------------------------------

   void selectSolver(char* name);
   void selectPreconditioner(char* name);

   // ----------------------------------------------------------------------
   // HYPRE specific private functions
   // ----------------------------------------------------------------------

   void  setupPCGPrecon();
   void  setupGMRESPrecon();
   void  setupBiCGSTABPrecon();
   void  setupBiCGSTABLPrecon();
   void  setupTFQmrPrecon();
   void  setupBiCGSPrecon();
   void  solveUsingBoomeramg(int&);
   void  solveUsingSuperLU(int&);
   void  solveUsingSuperLUX(int&);
   void  solveUsingY12M(int&);
   void  solveUsingAMGe(int&);
   void  buildSlideReducedSystemPartA(int*,int*,int,int,int*,int*);
   void  buildSlideReducedSystemPartB(int*,int*,int,int,int*,int*,
                                      HYPRE_ParCSRMatrix *);
   void  buildSlideReducedSystemPartC(int*,int*,int,int,int*,int*,
                                      HYPRE_ParCSRMatrix);

 private:        //variables

   // ----------------------------------------------------------------------
   // parallel communication information and output levels
   // ----------------------------------------------------------------------

   MPI_Comm        comm_;               // MPI communicator
   int             numProcs_;           // number of processors
   int             mypid_;              // my processor ID
   int             HYOutputLevel_;      // control print information

   // ----------------------------------------------------------------------
   // for storing information about how to load matrix directly
   // ----------------------------------------------------------------------

   int             mapFromSolnFlag_;
   int             mapFromSolnLeng_;
   int             mapFromSolnLengMax_;
   int             *mapFromSolnList_;
   int             *mapFromSolnList2_;

   // ----------------------------------------------------------------------
   // matrix and vectors
   // ----------------------------------------------------------------------

   HYPRE_IJMatrix  HYA_;                // the system matrix
   HYPRE_IJVector  HYb_;                // the current RHS 
   HYPRE_IJVector  *HYbs_;              // an array of RHSs
   HYPRE_IJVector  HYx_;                // the solution vector
   HYPRE_IJVector  HYr_;                // temporary vector for residual
   HYPRE_IJVector  *HYpxs_;             // an array of previous solutions
   HYPRE_IJVector  *HYpbs_;             // an array of previous rhs
   int             numGlobalRows_;
   int             localStartRow_;
   int             localEndRow_;
   int             *rowLengths_;
   int             **colIndices_;
   double          **colValues_;

   // ----------------------------------------------------------------------
   // matrix and vectors for reduction
   // ----------------------------------------------------------------------

   HYPRE_IJMatrix  reducedA_;           // matrix for reduction
   HYPRE_IJVector  reducedB_;           // RHS vector for reduction
   HYPRE_IJVector  reducedX_;           // solution vector for reduction
   HYPRE_IJVector  reducedR_;           // temporary vector for reduction
   HYPRE_IJMatrix  HYA21_;              // (2,1) block in reduction
   HYPRE_IJMatrix  HYA12_;              // (1,2) block in reduction
   int             A21NRows_;           // number of rows in (2,1) block
   int             A21NCols_;           // number of cols in (2,1) block
   int             reducedAStartRow_;   // Nrows in reduced system
   HYPRE_IJMatrix  HYinvA22_;           // inv(A22) in slide reduction

   // ----------------------------------------------------------------------
   // pointers to current matrix and vectors for solver
   // ----------------------------------------------------------------------

   HYPRE_IJMatrix  currA_;
   HYPRE_IJVector  currB_;
   HYPRE_IJVector  currX_;
   HYPRE_IJVector  currR_;
   int             currentRHS_;
   int             *rhsIDs_;
   int             numRHSs_;

   // ----------------------------------------------------------------------
   // various flags
   // ----------------------------------------------------------------------

   int             matrixVectorsCreated_;
   int             systemAssembled_;
   int             slideReduction_;
   int             schurReduction_;
   int             schurReductionCreated_;
   int             projectionScheme_;
   int             projectSize_;
   int             projectCurrSize_;

   // ----------------------------------------------------------------------
   // variables for slide reduction
   // ----------------------------------------------------------------------

   int             *selectedList_;
   int             *selectedListAux_;
   int             nConstraints_;
   int             *constrList_;

   // ----------------------------------------------------------------------
   // variables for the selected solver and preconditioner
   // ----------------------------------------------------------------------

   char            *HYSolverName_;
   HYPRE_Solver    HYSolver_;
   HYsolverID      HYSolverID_;
   int             gmresDim_;
   int             maxIterations_;
   int             finalResNorm_;
   double          tolerance_;
   int             normAbsRel_;

   char            *HYPreconName_;
   HYPRE_Solver    HYPrecon_;
   HYpreconID      HYPreconID_;
   int             HYPreconReuse_;
   int             HYPreconSetup_;

   // ----------------------------------------------------------------------
   // preconditioner specific variables
   // ----------------------------------------------------------------------

   int             amgCoarsenType_;
   int             amgMaxIter_;
   int             amgMeasureType_;
   int             amgNumSweeps_[4];
   int             amgRelaxType_[4];
   double          amgRelaxWeight_[25];
   double          amgStrongThreshold_;
   int             amgSystemSize_;
   int             pilutFillin_;
   double          pilutDropTol_;
   int             pilutMaxNnzPerRow_;
   int             parasailsSym_;
   double          parasailsThreshold_;
   int             parasailsNlevels_;
   double          parasailsFilter_;
   double          parasailsLoadbal_;
   int             parasailsReuse_;
   int             mlMethod_;
   int             mlNumPreSweeps_;
   int             mlNumPostSweeps_;
   int             mlPresmootherType_;
   int             mlPostsmootherType_;
   double          mlRelaxWeight_;
   double          mlStrongThreshold_;
   int             mlCoarseSolver_;
   int             mlCoarsenScheme_;
   int             superluOrdering_;
   char            superluScale_[1];
   double          ddilutFillin_;
   double          ddilutDropTol_;
   double          ddictFillin_;
   double          ddictDropTol_;
   double          schwarzFillin_;
   int             schwarzNblocks_;
   int             schwarzBlksize_;
   int             polyOrder_;
   int             euclidargc_;
   char            **euclidargv_;

   // ----------------------------------------------------------------------
   // map and others
   // ----------------------------------------------------------------------

   void            *fegrid;
   Lookup          *lookup_;
   int             haveLookup_;
   double          **projectionMatrix_; 

   // ----------------------------------------------------------------------
   // temporary functions for testing purposes
   // ----------------------------------------------------------------------

   friend void fei_hypre_test(int argc, char *argv[]);
   friend void fei_hypre_domaindecomposition(int argc, char *argv[]);

};

#endif
