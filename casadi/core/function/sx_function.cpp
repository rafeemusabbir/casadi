/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010-2014 Joel Andersson, Joris Gillis, Moritz Diehl,
 *                            K.U. Leuven. All rights reserved.
 *    Copyright (C) 2011-2014 Greg Horn
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include "sx_function_internal.hpp"
#include <limits>
#include <stack>
#include <deque>
#include <fstream>
#include <sstream>
#include "../std_vector_tools.hpp"
#include "../sx/sx_node.hpp"
#include "../sx/sx_tools.hpp"
#include "mx_function.hpp"

namespace casadi {

  using namespace std;


  SXFunction::SXFunction() {
  }

  SXFunction::SXFunction(const vector<SX>& arg, const vector<SX>& res) {
    assignNode(new SXFunctionInternal(arg, res));
  }

  SXFunction::SXFunction(const vector<SX>& arg, const IOSchemeVector<SX>& res) {
    assignNode(new SXFunctionInternal(arg, res.data));
    setOption("output_scheme", res.scheme.v());
  }

  SXFunction::SXFunction(const IOSchemeVector<SX>& arg, const vector<SX>& res) {
    assignNode(new SXFunctionInternal(arg.data, res));
    setOption("input_scheme", arg.scheme.v());
  }

  SXFunction::SXFunction(const IOSchemeVector<SX>& arg, const IOSchemeVector<SX>& res) {
    assignNode(new SXFunctionInternal(arg.data, res.data));
    setOption("input_scheme", arg.scheme.v());
    setOption("output_scheme", res.scheme.v());
  }

  SXFunction::SXFunction(const std::string& name, const std::vector<SX>& arg,
                         const std::vector<SX>& res, const Dictionary& opts) {
    assignNode(new SXFunctionInternal(arg, res));
    setOption("name", name);
    setOption(opts);
    init();
  }

#ifdef USE_CXX11
  SXFunction::SXFunction(const std::string& name, std::initializer_list<SX> arg,
                         std::initializer_list<SX> res, const Dictionary& opts) {
    assignNode(new SXFunctionInternal(vector<SX>(arg), vector<SX>(res)));
    setOption("name", name);
    setOption(opts);
    init();
  }

  SXFunction::SXFunction(const std::string& name, std::vector<SX> arg,
                         std::initializer_list<SX> res, const Dictionary& opts) {
    assignNode(new SXFunctionInternal(arg, vector<SX>(res)));
    setOption("name", name);
    setOption(opts);
    init();
  }

  SXFunction::SXFunction(const std::string& name, std::initializer_list<SX> arg,
                         std::vector<SX> res, const Dictionary& opts) {
    assignNode(new SXFunctionInternal(vector<SX>(arg), res));
    setOption("name", name);
    setOption(opts);
    init();
  }
#endif // USE_CXX11

  const SXFunctionInternal* SXFunction::operator->() const {
    return static_cast<const SXFunctionInternal*>(Function::operator->());
  }

  SXFunctionInternal* SXFunction::operator->() {
    return static_cast<SXFunctionInternal*>(Function::operator->());
  }

  bool SXFunction::testCast(const SharedObjectNode* ptr) {
    return dynamic_cast<const SXFunctionInternal*>(ptr)!=0;
  }

  SX SXFunction::jac(int iind, int oind, bool compact, bool symmetric) {
    return (*this)->jac(iind, oind, compact, symmetric);
  }

  SX SXFunction::grad(int iind, int oind) {
    return (*this)->grad(iind, oind);
  }

  SX SXFunction::tang(int iind, int oind) {
    return (*this)->tang(iind, oind);
  }

  SX SXFunction::hess(int iind, int oind) {
    return (*this)->hess(iind, oind);
  }

  const SX& SXFunction::inputExpr(int ind) const {
    return (*this)->inputv_.at(ind);
  }

  const SX& SXFunction::outputExpr(int ind) const {
    return (*this)->outputv_.at(ind);
  }

  const std::vector<SX>& SXFunction::inputExpr() const {
    return (*this)->inputv_;
  }

  const std::vector<SX> & SXFunction::outputExpr() const {
    return (*this)->outputv_;
  }

  const vector<ScalarAtomic>& SXFunction::algorithm() const {
    return (*this)->algorithm_;
  }

  int SXFunction::countNodes() const {
    assertInit();
    return algorithm().size() - nnzOut();
  }

  void SXFunction::clearSymbolic() {
    (*this)->clearSymbolic();
  }

  SXFunction::SXFunction(const MXFunction& f) {
    MXFunction f2 = f;
    SXFunction t = f2.expand();
    assignNode(t.get());
  }

  SXFunction::SXFunction(const Function& f) {
    const SXFunctionInternal* temp = dynamic_cast<const SXFunctionInternal*>(f.get());
    if (temp) {
      assignNode(temp->clone());
    } else {
      MXFunction f2(f);
      SXFunction t = f2.expand();
      assignNode(t.get());
    }
  }

  SX SXFunction::getFree() const {
    return (*this)->free_vars_;
  }

  int SXFunction::getWorkSize() const {
    return (*this)->sz_w();
  }

} // namespace casadi
