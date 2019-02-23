/*
 *
 *  $Id$
 *
 *  This file is part of the Virtual Leaf.
 *
 *  The Virtual Leaf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The Virtual Leaf is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Virtual Leaf.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2010 Roeland Merks.
 *
 */

#ifndef _FAR_MEM_5_h_
#define _FAR_MEM_5_h_

template <class _Arg1,  class _Result>
  struct my_1_function
  {
    typedef _Arg1 argument_type1;
    typedef _Result result_type;  ///<  result_type is the return type
  };
template <class Result, class Type,  class Param1>
  class far_1_arg_mem_fun_t : public my_1_function<Param1, Result> {
 public:
    explicit far_1_arg_mem_fun_t(Type &ir_typ, Result (Type::*i_pmf)(Param1)) {
      m_ptyp = &ir_typ;
      m_pmf = i_pmf;
    };
    Result operator()(Param1 i_prm1) { return (m_ptyp->*(m_pmf))(i_prm1); };

 protected:
    Type *m_ptyp;
    Result (Type::*m_pmf)(Param1);
};

template <class Result, class Type, 
  class Param1>far_1_arg_mem_fun_t<Result,Type,Param1> far_1_arg_mem_fun(Type &ir_typ, Result (Type::*i_pmf)(Param1) )
{
  return far_1_arg_mem_fun_t<Result,Type,Param1>(ir_typ,i_pmf);
}

template <class _Arg1, class _Arg2,  class _Result>
  struct my_2_function
  {
    typedef _Arg1 argument_type1;
    typedef _Arg2 argument_type2;
    typedef _Result result_type;  ///<  result_type is the return type
  };

template <class Result, class Type,  class Param1, class Param2>
  class far_2_arg_mem_fun_t : public my_2_function<Param1, Param2, Result> {
 public:
    explicit far_2_arg_mem_fun_t(Type &ir_typ, Result (Type::*i_pmf)(Param1, Param2)) {
      m_ptyp = &ir_typ;
      m_pmf = i_pmf;
    };
    Result operator()(Param1 i_prm1, Param2 i_prm2) { return (m_ptyp->*(m_pmf))(i_prm1, i_prm2); };

 protected:
    Type *m_ptyp;
    Result (Type::*m_pmf)(Param1, Param2);
};

template <class Result, class Type, class Param1, 
  class Param2>far_2_arg_mem_fun_t<Result,Type,Param1, Param2> far_2_arg_mem_fun(Type &ir_typ, Result (Type::*i_pmf)(Param1, Param2) ) {
  return far_2_arg_mem_fun_t<Result,Type,Param1, Param2>(ir_typ,i_pmf);
}

template <class _Arg1, class _Arg2, class _Arg3,  class _Result>
  struct my_3_function
  {
    typedef _Arg1 argument_type1;
    typedef _Arg2 argument_type2;
    typedef _Arg3 argument_type3;
    typedef _Result result_type;  ///<  result_type is the return type
  };

template <class Result, class Type,  class Param1, class Param2, class Param3>
  class far_3_arg_mem_fun_t : public my_3_function<Param1, Param2, Param3, Result> {
 public:
    explicit far_3_arg_mem_fun_t(Type &ir_typ, Result (Type::*i_pmf)(Param1, Param2, Param3)) {
      m_ptyp = &ir_typ;
      m_pmf = i_pmf;
    };
    Result operator()(Param1 i_prm1, Param2 i_prm2, Param3 i_prm3) { return (m_ptyp->*(m_pmf))(i_prm1, i_prm2, i_prm3); };

 protected:
    Type *m_ptyp;
    Result (Type::*m_pmf)(Param1, Param2, Param3);
};

template <class Result, class Type, class Param1, class Param2, 
class Param3>far_3_arg_mem_fun_t<Result,Type,Param1, Param2, Param3> far_3_arg_mem_fun(Type &ir_typ, Result (Type::*i_pmf)(Param1, Param2, Param3) ) {
  return far_3_arg_mem_fun_t<Result,Type,Param1, Param2, Param3>(ir_typ,i_pmf);
}

template <class _Arg1, class _Arg2, class _Arg3, class _Arg4,  class _Result>
  struct my_4_function
  {
    typedef _Arg1 argument_type1;
    typedef _Arg2 argument_type2;
    typedef _Arg3 argument_type3;
    typedef _Arg4 argument_type4;
    typedef _Result result_type;  ///<  result_type is the return type
  };

template <class Result, class Type,  class Param1, class Param2, class Param3, class Param4>
  class far_4_arg_mem_fun_t : public my_4_function<Param1, Param2, Param3, Param4, Result> {
 public:
    explicit far_4_arg_mem_fun_t(Type &ir_typ, Result (Type::*i_pmf)(Param1, Param2, Param3, Param4)) {
      m_ptyp = &ir_typ;
      m_pmf = i_pmf;
    };
    Result operator()(Param1 i_prm1, Param2 i_prm2, Param3 i_prm3, Param4 i_prm4) { return (m_ptyp->*(m_pmf))(i_prm1, i_prm2, i_prm3, i_prm4); };

 protected:
    Type *m_ptyp;
    Result (Type::*m_pmf)(Param1, Param2, Param3, Param4);
};

template <class Result, class Type, class Param1, class Param2, class Param3, class Param4>far_4_arg_mem_fun_t<Result,Type,Param1, Param2, Param3, Param4> far_4_arg_mem_fun(Type &ir_typ, Result (Type::*i_pmf)(Param1, Param2, Param3, Param4) ) {
  return far_4_arg_mem_fun_t<Result,Type,Param1, Param2, Param3, Param4>(ir_typ,i_pmf);
}

template <class _Arg1, class _Arg2, class _Arg3, class _Arg4, class _Arg5,  class _Result>
  struct my_5_function
  {
    typedef _Arg1 argument_type1;
    typedef _Arg2 argument_type2;
    typedef _Arg3 argument_type3;
    typedef _Arg4 argument_type4;
    typedef _Arg5 argument_type5;
    typedef _Result result_type;  ///<  result_type is the return type
  };

template <class Result, class Type,  class Param1, class Param2, class Param3, class Param4, class Param5>
  class far_5_arg_mem_fun_t : public my_5_function<Param1, Param2, Param3, Param4, Param5, Result> {
 public:
    explicit far_5_arg_mem_fun_t(Type &ir_typ, Result (Type::*i_pmf)(Param1, Param2, Param3, Param4, Param5)) {
      m_ptyp = &ir_typ;
      m_pmf = i_pmf;
    };
    Result operator()(Param1 i_prm1, Param2 i_prm2, Param3 i_prm3, Param4 i_prm4, Param5 i_prm5) { return (m_ptyp->*(m_pmf))(i_prm1, i_prm2, i_prm3, i_prm4, i_prm5); };

 protected:
    Type *m_ptyp;
    Result (Type::*m_pmf)(Param1, Param2, Param3, Param4, Param5);
};

template <class Result, class Type, class Param1, class Param2, class Param3, class Param4, class Param5>far_5_arg_mem_fun_t<Result,Type,Param1, Param2, Param3, Param4, Param5> far_5_arg_mem_fun(Type &ir_typ, Result (Type::*i_pmf)(Param1, Param2, Param3, Param4, Param5) ) {
  return far_5_arg_mem_fun_t<Result,Type,Param1, Param2, Param3, Param4, Param5>(ir_typ,i_pmf);
}

#endif

/* finis */
