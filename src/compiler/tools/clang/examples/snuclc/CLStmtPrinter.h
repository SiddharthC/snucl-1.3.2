/*****************************************************************************/
/*                                                                           */
/* Copyright (c) 2011-2013 Seoul National University.                        */
/* All rights reserved.                                                      */
/*                                                                           */
/* Redistribution and use in source and binary forms, with or without        */
/* modification, are permitted provided that the following conditions        */
/* are met:                                                                  */
/*   1. Redistributions of source code must retain the above copyright       */
/*      notice, this list of conditions and the following disclaimer.        */
/*   2. Redistributions in binary form must reproduce the above copyright    */
/*      notice, this list of conditions and the following disclaimer in the  */
/*      documentation and/or other materials provided with the distribution. */
/*   3. Neither the name of Seoul National University nor the names of its   */
/*      contributors may be used to endorse or promote products derived      */
/*      from this software without specific prior written permission.        */
/*                                                                           */
/* THIS SOFTWARE IS PROVIDED BY SEOUL NATIONAL UNIVERSITY "AS IS" AND ANY    */
/* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED */
/* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE    */
/* DISCLAIMED. IN NO EVENT SHALL SEOUL NATIONAL UNIVERSITY BE LIABLE FOR ANY */
/* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL        */
/* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS   */
/* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)     */
/* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,       */
/* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN  */
/* ANY WAY OUT OF THE USE OF THIS  SOFTWARE, EVEN IF ADVISED OF THE          */
/* POSSIBILITY OF SUCH DAMAGE.                                               */
/*                                                                           */
/* Contact information:                                                      */
/*   Center for Manycore Programming                                         */
/*   School of Computer Science and Engineering                              */
/*   Seoul National University, Seoul 151-744, Korea                         */
/*   http://aces.snu.ac.kr                                                   */
/*                                                                           */
/* Contributors:                                                             */
/*   Sangmin Seo, Jungwon Kim, Gangwon Jo, Jun Lee, Jeongho Nah,             */
/*   Jungho Park, Junghyun Kim, and Jaejin Lee                               */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* This file is based on the SNU-SAMSUNG OpenCL Compiler and is distributed  */
/* under GNU General Public License.                                         */
/* See LICENSE.SNU-SAMSUNG_OpenCL_C_Compiler.TXT for details.                */
/*****************************************************************************/

#ifndef SNUCLC_CLSTMTPRINTER_H
#define SNUCLC_CLSTMTPRINTER_H

#include "clang/AST/StmtVisitor.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/PrettyPrinter.h"
#include "llvm/Support/Format.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"

//===--------------------------------------------------------------------===//
// CLStmtPrinter Visitor
//===--------------------------------------------------------------------===//

namespace clang {
namespace snuclc {

class CLStmtPrinter : public StmtVisitor<CLStmtPrinter> {
  llvm::raw_ostream &OS;
  ASTContext &Context;
  unsigned IndentLevel;
  PrinterHelper* Helper;
  PrintingPolicy Policy;

public:
  CLStmtPrinter(llvm::raw_ostream &os, ASTContext &C, PrinterHelper* helper,
              const PrintingPolicy &Policy,
              unsigned Indentation = 0)
    : OS(os), Context(C), IndentLevel(Indentation), Helper(helper),
      Policy(Policy) {}

  void PrintStmt(Stmt *S) {
    PrintStmt(S, Policy.Indentation);
  }

  void PrintStmt(Stmt *S, int SubIndent) {
    IndentLevel += SubIndent;
    if (S && isa<Expr>(S)) {
      // If this is an expr used in a stmt context, indent and newline it.
      Indent();
      Visit(S);
      OS << ";\n";
    } else if (S) {
      Visit(S);
    } else {
      Indent() << "<<<NULL STATEMENT>>>\n";
    }
    IndentLevel -= SubIndent;
  }

  void PrintRawCompoundStmt(CompoundStmt *S);
  void PrintRawDecl(Decl *D);
  void PrintRawDeclStmt(DeclStmt *S);
  void PrintRawIfStmt(IfStmt *If);
  void PrintRawCXXCatchStmt(CXXCatchStmt *Catch);
  void PrintCallArgs(CallExpr *E);

  void PrintExpr(Expr *E) {
    if (E)
      Visit(E);
    else
      OS << "<null expr>";
  }

  llvm::raw_ostream &Indent(int Delta = 0) {
    for (int i = 0, e = IndentLevel+Delta; i < e; ++i)
      OS << "  ";
    return OS;
  }

  void Visit(Stmt* S) {
    if (Helper && Helper->handledStmt(S,OS))
        return;
    else StmtVisitor<CLStmtPrinter>::Visit(S);
  }
  
  void VisitStmt(Stmt *Node) LLVM_ATTRIBUTE_UNUSED {
    Indent() << "<<unknown stmt type>>\n";
  }
  void VisitExpr(Expr *Node) LLVM_ATTRIBUTE_UNUSED {
    OS << "<<unknown expr type>>";
  }
  void VisitCXXNamedCastExpr(CXXNamedCastExpr *Node);

#define ABSTRACT_STMT(CLASS)
#define STMT(CLASS, PARENT) \
  void Visit##CLASS(CLASS *Node);
#include "clang/AST/StmtNodes.inc"
};

class CLStmt {
public:
  static void printPretty(llvm::raw_ostream &OS, ASTContext& Context,
                          PrinterHelper* Helper,
                          const PrintingPolicy &Policy,
                          unsigned Indentation,
                          Stmt *S);
};

} //end namespace snuclc
} //end namespace clang

#endif //SNUCLC_CLSTMTPRINTER_H
