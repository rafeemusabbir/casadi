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


#include "mx_function_internal.hpp"
#include "../mx/mx_node.hpp"
#include "../std_vector_tools.hpp"
#include "../mx/mx_tools.hpp"

#include <stack>
#include <typeinfo>

using namespace std;

namespace casadi {

  bool MXFunction::testCast(const SharedObjectNode* ptr) {
    return dynamic_cast<const MXFunctionInternal*>(ptr)!=0;
  }

  MXFunction::MXFunction() {
  }

  MXFunction::MXFunction(const Function& function) {
    const MXFunctionInternal* temp = dynamic_cast<const MXFunctionInternal*>(function.get());
    if (!temp) casadi_error("MXFunction(Function)::input Function cannot be cast into MXFunction");
    assignNode(temp->clone());
  }

  MXFunction::MXFunction(const std::vector<MX>& arg, const std::vector<MX>& res) {
    assignNode(new MXFunctionInternal(arg, res));
  }

  MXFunction::MXFunction(const std::vector<MX>& arg, const IOSchemeVector<MX>& res) {
    assignNode(new MXFunctionInternal(arg, res.data));
    setOption("output_scheme", res.scheme.v());
  }

  MXFunction::MXFunction(const IOSchemeVector<MX>& arg, const std::vector<MX>& res) {
    assignNode(new MXFunctionInternal(arg.data, res));
    setOption("input_scheme", arg.scheme.v());
  }


  MXFunction::MXFunction(const IOSchemeVector<MX>& arg, const IOSchemeVector<MX>& res) {
    assignNode(new MXFunctionInternal(arg.data, res.data));
    setOption("input_scheme", arg.scheme.v());
    setOption("output_scheme", res.scheme.v());
  }

  MXFunction::MXFunction(const std::string& name, const std::vector<MX>& arg,
                         const std::vector<MX>& res, const Dictionary& opts) {
    assignNode(new MXFunctionInternal(arg, res));
    setOption("name", name);
    setOption(opts);
    init();
  }

#ifdef USE_CXX11
  MXFunction::MXFunction(const std::string& name, std::initializer_list<MX> arg,
                         std::initializer_list<MX> res, const Dictionary& opts) {
    assignNode(new MXFunctionInternal(vector<MX>(arg), vector<MX>(res)));
    setOption("name", name);
    setOption(opts);
    init();
  }

  MXFunction::MXFunction(const std::string& name, std::vector<MX> arg,
                         std::initializer_list<MX> res, const Dictionary& opts) {
    assignNode(new MXFunctionInternal(arg, vector<MX>(res)));
    setOption("name", name);
    setOption(opts);
    init();
  }

  MXFunction::MXFunction(const std::string& name, std::initializer_list<MX> arg,
                         std::vector<MX> res, const Dictionary& opts) {
    assignNode(new MXFunctionInternal(vector<MX>(arg), res));
    setOption("name", name);
    setOption(opts);
    init();
  }
#endif // USE_CXX11

  const MXFunctionInternal* MXFunction::operator->() const {
    return static_cast<const MXFunctionInternal*>(Function::operator->());
  }

  MXFunctionInternal* MXFunction::operator->() {
    return static_cast<MXFunctionInternal*>(Function::operator->());
  }

  const MX& MXFunction::inputExpr(int ind) const {
    return (*this)->inputv_.at(ind);
  }

  const MX& MXFunction::outputExpr(int ind) const {
    return (*this)->outputv_.at(ind);
  }

  const std::vector<MX>& MXFunction::inputExpr() const {
    return (*this)->inputv_;
  }

  const std::vector<MX> & MXFunction::outputExpr() const {
    return (*this)->outputv_;
  }

  const std::vector<MXAlgEl>& MXFunction::algorithm() const {
    return (*this)->algorithm_;
  }

  int MXFunction::countNodes() const {
    assertInit();
    return algorithm().size();
  }

  MX MXFunction::jac(int iind, int oind, bool compact, bool symmetric) {
    return (*this)->jac(iind, oind, compact, symmetric);
  }

  MX MXFunction::grad(int iind, int oind) {
    return (*this)->grad(iind, oind);
  }

  MX MXFunction::tang(int iind, int oind) {
    return (*this)->tang(iind, oind);
  }

  SXFunction MXFunction::expand(const std::vector<SX>& inputv) {
    return (*this)->expand(inputv);
  }

  std::vector<MX> MXFunction::getFree() const {
    return (*this)->free_vars_;
  }

  int MXFunction::getWorkSize() const {
    return (*this)->workloc_.size()-1;
  }

  void MXFunction::generateLiftingFunctions(MXFunction& vdef_fcn, MXFunction& vinit_fcn) {
    (*this)->generateLiftingFunctions(vdef_fcn, vinit_fcn);
  }


} // namespace casadi

