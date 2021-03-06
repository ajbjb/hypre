/*BHEADER**********************************************************************
 * Copyright (c) 2008,  Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 * This file is part of HYPRE.  See file COPYRIGHT for details.
 *
 * HYPRE is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License (as published by the Free
 * Software Foundation) version 2.1 dated February 1999.
 *
 * $Revision$
 ***********************************************************************EHEADER*/




/******************************************************************************
 *
 * hypre_IJMatrix interface
 *
 *****************************************************************************/

#include "./_hypre_IJ_mv.h"

#include "../HYPRE.h"

/*--------------------------------------------------------------------------
 * hypre_IJMatrixGetRowPartitioning
 *--------------------------------------------------------------------------*/

/**
Returns a pointer to the row partitioning 

@return integer error code
@param IJMatrix [IN]
The ijmatrix to be pointed to.
*/

HYPRE_Int
hypre_IJMatrixGetRowPartitioning( HYPRE_IJMatrix matrix ,
				  HYPRE_Int    **row_partitioning )
{
   hypre_IJMatrix *ijmatrix = (hypre_IJMatrix *) matrix;

   if (!ijmatrix)
   {
      hypre_error_w_msg(HYPRE_ERROR_GENERIC,"Variable ijmatrix is NULL -- hypre_IJMatrixGetRowPartitioning\n");
      return hypre_error_flag;
   }

   if ( hypre_IJMatrixRowPartitioning(ijmatrix))
      *row_partitioning = hypre_IJMatrixRowPartitioning(ijmatrix);
   else
   {
      hypre_error(HYPRE_ERROR_GENERIC);
      return hypre_error_flag;
   }

   return hypre_error_flag;
}
/*--------------------------------------------------------------------------
 * hypre_IJMatrixGetColPartitioning
 *--------------------------------------------------------------------------*/

/**
Returns a pointer to the column partitioning

@return integer error code
@param IJMatrix [IN]
The ijmatrix to be pointed to.
*/

HYPRE_Int
hypre_IJMatrixGetColPartitioning( HYPRE_IJMatrix matrix ,
				  HYPRE_Int    **col_partitioning )
{
   hypre_IJMatrix *ijmatrix = (hypre_IJMatrix *) matrix;

   if (!ijmatrix)
   {
      hypre_error_w_msg(HYPRE_ERROR_GENERIC,"Variable ijmatrix is NULL -- hypre_IJMatrixGetColPartitioning\n");
      return hypre_error_flag;
   }

   if ( hypre_IJMatrixColPartitioning(ijmatrix))
      *col_partitioning = hypre_IJMatrixColPartitioning(ijmatrix);
   else
   {
      hypre_error(HYPRE_ERROR_GENERIC);
      return hypre_error_flag;
   }

   return hypre_error_flag;
}
/*--------------------------------------------------------------------------
 * hypre_IJMatrixSetObject
 *--------------------------------------------------------------------------*/

HYPRE_Int 
hypre_IJMatrixSetObject( HYPRE_IJMatrix  matrix, 
                         void           *object )
{
   hypre_IJMatrix *ijmatrix = (hypre_IJMatrix *) matrix;

   if (hypre_IJMatrixObject(ijmatrix) != NULL)
   {
      /*hypre_printf("Referencing a new IJMatrix object can orphan an old -- ");
      hypre_printf("hypre_IJMatrixSetObject\n");*/
      hypre_error(HYPRE_ERROR_GENERIC);
      return hypre_error_flag;
   }

   hypre_IJMatrixObject(ijmatrix) = object;

   return hypre_error_flag;
}
