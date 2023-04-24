本方法支持任意普通函数，仿函数，lambda表达式，普通类成员函数，const类成员函数，以及静态成员函数。支持可变参数，支持基类成员函数，支持右值传参。

先给出代码：


```cpp
#pragma once

#include <iostream>
#include <type_traits>

// member function template
template<typename R, typename T, typename... ARGS>
struct FunCall
{
    using FunType = R(T::*)(ARGS...);
    FunCall(T* pkObj, FunType pFun)
        : m_pObj(pkObj)
        , m_pFun(pFun) 
    {
    }

    R DoInvoke(ARGS&&... args)
    {
        return (m_pObj->*m_pFun)(std::forward<ARGS>(args)...);
    };
private:
    T* m_pObj = nullptr;
    FunType m_pFun = nullptr;
};

template<typename R, typename T, typename... ARGS>
auto CreateAnyMemberFunCall(T* pObj,R(T::*pFun)(ARGS...))
        ->decltype(FunCall<R, T, ARGS...>(pObj,pFun))
{
    return FunCall<R, T, ARGS...>(pObj,pFun);
}

// use the enable for detech
template<bool, typename T = void>
struct EnableIf
{
};

template<typename T>
struct EnableIf<true,T>
{
    using Type = T;
};

template<typename Base, typename Drived>
struct IsInherit
{
    using B_D_T = std::decay_t<Base>;
    using D_D_T = std::decay_t<Drived>;
    static auto Check(B_D_T*)->int;
    static auto Check(...)->void;
    static constexpr bool Value = !std::is_void_v<decltype(Check((D_D_T*)0))>;
};

template<typename Base,  typename Drived, 
        typename EnableIf<IsInherit<Base,Drived>::Value,std::decay_t<Drived>>::Type* = nullptr>
auto GetRightMemberType(Drived&& obj) -> decltype(static_cast<Drived&&>(obj))
{
    return static_cast<Drived&&>(obj);
}

template<typename Base, typename Drived,
    typename EnableIf<!IsInherit<Base,Drived>::Value,std::decay_t<Drived>>::Type* = nullptr>
auto GetRightMemberType(Drived&& obj) -> decltype(*(static_cast<Drived&&>(obj)))
{
    return *(static_cast<Drived&&>(obj));
}

template<typename F, typename... ARGS>
auto CommonInvokeCall(F&& funObj, ARGS&&... args)
        ->decltype(std::forward<F>(funObj)(std::forward<ARGS>(args)...))
{
    return std::forward<F>(funObj)(std::forward<ARGS>(args)...);
}


//member function
template<typename R, typename T, typename C, typename... ARGS>
auto CommonInvokeCall(R (T::*pfMem)(ARGS...), C&& obj, ARGS&&... args)
        ->decltype((GetRightMemberType<T>(std::forward<C>(obj)).*pfMem)(std::forward<ARGS>(args)...))
{

    return (GetRightMemberType<T>(std::forward<C>(obj)).*pfMem)(std::forward<ARGS>(args)...);
}

template<typename R, typename T, typename C, typename... ARGS>
auto CommonInvokeCall(R (T::*pfMem)(ARGS...) const, C&& obj, ARGS&&... args)
        ->decltype((GetRightMemberType<T>(std::forward<C>(obj)).*pfMem)(std::forward<ARGS>(args)...))
{

    return (GetRightMemberType<T>(std::forward<C>(obj)).*pfMem)(std::forward<ARGS>(args)...);
}

// test class
class BaseClassForFunCall
{
public:
    int AddTwoNum(int a,int b) 
    {
        std::cout << "BaseAddTwoNum" << std::endl;
        return a+b;
    }
};

class ClassForFunCall : public BaseClassForFunCall
{
public:
    int AddTwoNum(int a,int b) 
    {
        std::cout << "DrivedAddTwoNum" << std::endl;
        return a+b;
    }

    int AddTwoNumConst(int a,int b) const
    {
        std::cout << "const addtwonum" << std::endl;
        return a+b;
    }

    static int AddTwoNumStatic(int a, int b)
    {
        std::cout << "static addtwofloatnum" << std::endl;
        return a+b;	
    }
};


struct TestFunctor
{
    std::string operator()(const std::string& a,const std::string& b)
    {
        std::cout << "TestFunctor Start" << a.c_str() << std::endl;
        std::cout << "a string:" << a.c_str() << std::endl;
        std::cout << "b string" << b.c_str() << std::endl;
        return a+b;
    }
};

std::string TestFun(const std::string& a,const std::string& b)
{
    std::cout << "TestFun Start" << a.c_str() << std::endl;
    std::cout << "a string:" << a.c_str() << std::endl;
    std::cout << "b string" << b.c_str() << std::endl;
    return a+b;
}

static void TestInvoke()
{
	// test for common member fuction
	ClassForFunCall c1;
	auto caller = CreateAnyMemberFunCall(&c1,&ClassForFunCall::AddTwoNum);
	caller.DoInvoke(12,15);

	// test for functor
	TestFunctor funobj;
	const std::string a = "hello ",b = "c++ world";
	auto aret1 = CommonInvokeCall(funobj, "hello ","c++ world");
	std::cout << aret1.c_str() << std::endl;
	auto aret11 = CommonInvokeCall(funobj,a,b);
	std::cout << aret11.c_str() << std::endl;

	//test for lambda
	CommonInvokeCall([](const std::string& a,const std::string& b){
        std::cout << "lambda " << a.c_str() << b.c_str() << std::endl;
        return a+b;
    },
    a,b);

    // test for common function
	auto aret2 = CommonInvokeCall(TestFun,a,b);
	std::cout << aret2.c_str() << std::endl;


	// test for static member function
	auto aret3 = CommonInvokeCall(ClassForFunCall::AddTwoNumStatic,22,100);
	std::cout << aret3 << std::endl;

	// test for common member function
	auto aret4 = CommonInvokeCall(&ClassForFunCall::AddTwoNum,c1,100,200);
	std::cout << aret4 << std::endl;

	// test for common member function
	auto aret5 = CommonInvokeCall(&ClassForFunCall::AddTwoNumConst,c1,100,300);
	std::cout << aret5 << std::endl;


	// // test for common base member function
	auto aret6 = CommonInvokeCall(&BaseClassForFunCall::AddTwoNum,c1,100,400);
	std::cout << aret6 << std::endl;


	ClassForFunCall* pkC1 = &c1;
	auto aret7 = CommonInvokeCall(&ClassForFunCall::AddTwoNum,pkC1,100,500);
	std::cout << aret7 << std::endl;

    BaseClassForFunCall* pkC2 = &c1;
	auto aret8 = CommonInvokeCall(&BaseClassForFunCall::AddTwoNum,pkC2,100,500);
	std::cout << aret7 << std::endl;
}

int main()
{
    TestInvoke();
    return 0;
}

```
## 1.最普通的类任意成员函数

### 1.1 先写一个简单的成员函数类模板

我们先写出一个最普通的类成员函数对应的AnyMemberFunCall模板类，

<typename R,typename T,typename... ARGS>
有三个模板参数，R表标函数返回值，T表示class类的型别，ARGS是一个可变模板参数，用来表示成员函数的参数类型。

using FunType =R(T::*)(ARGS...);
接着就定义好了函数类型，参数可以是任意的，用了可变参数，只是这个函数只能是普通成员函数，且不能是const成员函数。

T* m_pkObj =nullptr; 		
FunType m_pfFun =nullptr;
这个模板类给了两个成员，一个T类型对象指针，用来call函数时传this指针，一个是前面函数对象，不过具体来说是成员函数地址。

构造函数和调用都比较简单，调用直接
(m_pkObj->*m_pfFun)(std::forward<ARGS>(args)...);
形式，只是参数用了完美转发，保留了原来的参数类型。


### 1.2 参数的自动推导

这个类有个不好地方，那就要显式的声明模板参数，这样看起来有点low，能不能不显式的声明呢？
可以的，用模板函数的自动推断。那也就是引入一个中间层过渡一下喽，对，用到计算机名言，没有什么问题不是加一层中间层解决不了，如果有，那就再加一层。

template<typename R,typename T,typename... ARGS>
auto CreateAnyMemberFunCall(T* pkObj,R(T::*pkFun)(ARGS...))
->decltype(AnyMemberFunCall(pkObj,pkFun))
分析一下这句，一个函数模板CreateAnyMemberFunCall，第一个参数是对象指针，也就是我传了一个对象地址，它就能推出T的类型，搞定一个参数了。第二个是成员函数指针，如果给了一个成员函数，一个明确的成员函数必然知道返回值，类型，参数类型，那么也就知道了R，T，ARGS...,也就是说参数都能推断出来了，很好，貌似能满足。

不过这里也会对成员函数所属类型T与pkobj类型T进行匹配，如果不匹配就会报错，比如我们成员对象指针随意加个const,理论上没有问题，但对于模板来说就会出问题，比如

ClassForFunCall c1;
const ClassForFunCall* pkC1Const = &c1;
auto caller = CreateAnyMemberFunCall(&c1,&ClassForFunCall::AddTwoNum);
auto caller1 = CreateAnyMemberFunCall(pkC1Const,&ClassForFunCall::AddTwoNum);
对于caller1这行，第一个参数，推导出T = const ClassForFunCall;第二个函数指针参数推导出T= ClassForFunCall;显然两者不相等，必然caller1这行编译不过。

对于->decltype(AnyMemberFunCall(pkObj,pkFun)) 这句实际是尾置表达式，结合返回值auto，要编译器自动给出具体的类型，看一下代码，就是前面类模板的构造函数，也就是

AnyMemberFunCall<R,T,ARGS...> 这个类型，只是这个类型写在函数前面显得有点丑，且给定了类型，不够高大上。

### 1.3 带来的问题与思考

参数推导也讲完了？有什么问题，肯定有！

a.支持仿函数，支持lambda表达式，支持普通函数。。。
b.支持基类函数
c.我还想不管是类对象指针，还是普通类对象，都可以，现在限定在类对象指针

auto caller1 = CreateAnyMemberFunCall(&c1,&ClassForFunCall::BaseAddTwoNum);
std::cout << caller1.DoInvoke(100,200) << std::endl;
这里是编译不过的，不过也看编译器，MSVC可能检查不严，可能过，但Clang是不让通过的。原因BaseAddTwoNum是基类成员函数，编译器得到的R(T::*)(ARGS...)中的T是BaseClassForFunCall，而第一个参数得到的T是ClassForFunCall，所以给报错，也是正常的。



## 2 大决战，解决任意函数

### 2.1 解决普通函数，仿函数，lambda，成员静态函数问题

一般来说分为普通函数，仿函数，lambda，成员静态函数，这些都不需要this指针，形式可以抽样成一样的，有了这些，我们只需要略加处理即可。

 template<typename F,typename... ARGS>
 auto CommonInvokeCall(F&& funObj,ARGS&&... args)
    ->decltype(std::forward<F>(funObj)(std::forward<ARGS>(args)...))
{
	return std::forward<F>(funObj)(std::forward<ARGS>(args)...);
}

F是泛型，可以是任意类型函数，但调用要满足funObj(args...);这种形式，那么在推导具体参数时，普通函数，仿函数，lambda，成员静态函数都能满足，至于成员函数F的确可以推导，但调用不行，会失败。所以这一个表达式解决了一大部分类类型的。结合下面要介绍的成员函数，这里用到SFINAE相关知识。

//test for common function 普通函数
const std::string a = "hello ",b = "c++ world3";
auto aret2 = CommonInvokeCall(testfun1,a,b);
//test for lambda，bambda表达式
CommonInvokeCall([](const std::string& a,const std::string& b)->decltype(a+b)\
{std::cout << "lambda " << a.c_str() << b.c_str() << std::endl;return a+b;},a,b);
// test for static member function，静态成员函数
auto aret3 = CommonInvokeCall(ClassForFunCall::addtwofloatnum,22.42f,100.2f);
// test for functor，仿函数，不知道仿函数的，可以查一下
myfunctor funobj;
const std::string a1 = "hello ",b1 = "c++ world1";
auto aret1 = CommonInvokeCall(funobj,"hello ","c++ world1");

### 2.2 解决成员函数
分为普通成员函数，const成员函数，形式多了一个const，但意义相差就很大了。

template<typename R,typename T,typename... ARGS>
auto CreateInvokeCall(R (T::*pfMem)(ARGS...),T obj,ARGS&&... args)
		->decltype((obj.*pfMem)(std::forward<ARGS>(args)...))
{
	return (obj.*pfMem)(std::forward<ARGS>(args)...);
}

template<typename R,typename T,typename... ARGS>
auto CreateInvokeCall(R (T::*pfMem)(ARGS...) const,T obj,ARGS&&... args)
		->decltype((obj.*pfMem)(std::forward<ARGS>(args)...))
{
	return (obj.*pfMem)(std::forward<ARGS>(args)...);
}
形式基本这样就可以了，第二个是加了const的特化，针对const成员函数

### 2.3处理基类成员函数
也就是说基类成员函数对于我们的函数CreateInvokeCall(R(T::*pfMem)(ARGS...),T obj,ARGS&&... args) ，第一个pfMem所属类型T1和obj所属类型T2不一定相同，那好处理，非常简单，再加一个类型C就可以了。

template<typename R,typename T,typename C,typename... ARGS>
auto CommonInvokeCallTwo(R (T::*pfMem)(ARGS...),C&& obj,ARGS&&... args)
		->decltype((std::forward<C>(obj).*pfMem)(std::forward<ARGS>(args)...))
{

	return (std::forward<C>(obj).*pfMem)(std::forward<ARGS>(args)...);
}

template<typename R,typename T,typename C,typename... ARGS>
auto CommonInvokeCallTwo(R (T::*pfMem)(ARGS...) const,C&& obj,ARGS&&... args)
		->decltype((std::forward<C>(obj).*pfMem)(std::forward<ARGS>(args)...))
{

	return (std::forward<C>(obj).*pfMem)(std::forward<ARGS>(args)...);
}

我们可以测试一下
// test for common base member function
auto aret6 = CreateInvokeCall(&ClassForFunCall::BaseAddTwoNum,c1,100,200);
std::cout << aret6 << std::endl;

这时T被推导成BaseClassForFunCall，C被推导成ClassForFunCall，且后面c1能满足(c1.*BaseAddTwoNum)调用，编译不会出错，ok！

### 2.4自动处理类对象指针和普通类对象
最后一个问题，怎么可以支持无论C的obj对象是指针还是普通都ok呢，当然还是特化喽！

对于obj来说，如果是普通对象，直接来(obj.*pfn)(...); 就能访问了，但如果是指针，我们解一下引用就可以了，也就是((*obj).*pfn)(...); 多的这一步，也交给中间层吧，又用到中间层了。

template<bool bCheck,typename T = void>
struct MyEnableIf{};
template<typename T>
struct MyEnableIf<true,T>
{
	using Type = T;
};
template<typename B,typename D>
struct IsDClassFromBClass
{
	using B_D_T = std::decay_t<B>;
	using D_D_T = std::decay_t<D>;
	static auto Check(B_D_T*)->int;
	static auto Check(...)->void;
	static constexpr bool Value = !std::is_void_v<decltype(Check((D_D_T*)0))>;
};

先引入MyEnableIf，有两个参数，第一个bool，第二个是T，如果对于第一个参数不为真，就没有定义Type，为真才定义Type = T，注意这里第一个参数必须是编译期常量，也就编译的时间，就能计算出。这个功能常用来编译期选择与检查。

再看IsDClassFromBClass，有两个参数B，D，这个用了std::decayt退化，_t是C++14引入的，C++11可以直接typename decay<T>::type。模板传值时也会用到退化，这里std::decayt用来消除const，volatile,以及数组，得到相对原如的T类型。同理std::is_voidv用来判断是否是void类型，_v表示is_void<T>::Value;

这里函数Check只是需要声明就可以了，没有真正用到。有两个不同重载形式，第一个auto Check(B_D_T*)->int; 表示如果能转成B_D_T*类型的，就用这个版本，返回int，每二个auto Check(...)->void; 是个保底，任意类型返回void，对于编译器会先最优匹配，不能匹配时返回void版本。所以

static constexpr bool Value =!std::is_void_v<decltype(Check((D_D_T*)0))>;
这句是编译器常量，如果DDT*指针能转成BDT*指针就是int，由编译器判断指针能否转换，最后再返回bool值，也没有什么问题。

看最后的GetRightMemberType

template<typename B,typename D,
typename MyEnableIf<IsDClassFromBClass<B,D>::Value,std::decay_t<D>>::Type* = nullptr>
auto GetRightMemberType(D&& obj)->decltype(static_cast<D&&>(obj))
{
	return static_cast<D&&>(obj);
}

template<typename B,typename D,
typename MyEnableIf<!IsDClassFromBClass<B,D>::Value,std::decay_t<D>>::Type* = nullptr>
auto GetRightMemberType(D&& obj)->decltype(*(static_cast<D&&>(obj)))
{
	return *(static_cast<D&&>(obj));
}
有了上面知识，这里一点也不难，如果退化的D能转成B，这里说明D是一个普通类型，只需原路返回，如果转化失败，说明传的是指针，再模板推导IsDClassFromBClass会失败。如果是指针，我们就需要解一次引用，返回return*(static_cast<D&&>(obj)); 一切都搞定了。

我们给出最终版本

//member function
template<typename R,typename T,typename C,typename... ARGS>
auto CommonInvokeCall(R (T::*pfMem)(ARGS...),C&& obj,ARGS&&... args)
->decltype((GetRightMemberType<T>(std::forward<C>(obj)).*pfMem)(std::forward<ARGS>(args)...))
{
     return (GetRightMemberType<T>(std::forward<C>(obj)).*pfMem)(std::forward<ARGS>(args)...);
}

template<typename R,typename T,typename C,typename... ARGS>
auto CommonInvokeCall(R (T::*pfMem)(ARGS...) const,C&& obj,ARGS&&... args)
->decltype((GetRightMemberType<T>(std::forward<C>(obj)).*pfMem)(std::forward<ARGS>(args)...))
{
     return (GetRightMemberType<T>(std::forward<C>(obj)).*pfMem)(std::forward<ARGS>(args)...);
}


## 3.结束

本文 就到这结不了，用到一些模板技术，希望大家能喜欢，给个赞与关注。


## 4.附件
### 4.1 Qt的实现
```cpp

template<typename Func> struct FunctionPointer { enum {ArgumentCount = -1, IsPointerToMemberFunction = false}; };
template<class Obj, typename Ret, typename... Args> struct FunctionPointer<Ret (Obj::*) (Args...)>
{
	typedef Obj Object;
	typedef List<Args...>  Arguments;
	typedef Ret ReturnType;
	typedef Ret (Obj::*Function) (Args...);
	enum {ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = true};
	template <typename SignalArgs, typename R>
	static void call(Function f, Obj *o, void **arg) {
		FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
	}
};
template<class Obj, typename Ret, typename... Args> struct FunctionPointer<Ret (Obj::*) (Args...) const>
{
	typedef Obj Object;
	typedef List<Args...>  Arguments;
	typedef Ret ReturnType;
	typedef Ret (Obj::*Function) (Args...) const;
	enum {ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = true};
	template <typename SignalArgs, typename R>
	static void call(Function f, Obj *o, void **arg) {
		FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
	}
};
template<typename Ret, typename... Args> struct FunctionPointer<Ret (*) (Args...)>
{
	typedef List<Args...> Arguments;
	typedef Ret ReturnType;
	typedef Ret (*Function) (Args...);
	enum {ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = false};
	template <typename SignalArgs, typename R>
	static void call(Function f, void *, void **arg) {
		FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, arg);
	}
};
```

```cpp
template <typename, typename, typename, typename> struct FunctorCall;
template <int... II, typename... SignalArgs, typename R, typename Function>
struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, Function> {
	static void call(Function &f, void **arg) {
		f((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II+1]))...), ApplyReturnValue<R>(arg[0]);
	}
};
template <int... II, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, SlotRet (Obj::*)(SlotArgs...)> {
	static void call(SlotRet (Obj::*f)(SlotArgs...), Obj *o, void **arg) {
		(o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II+1]))...), ApplyReturnValue<R>(arg[0]);
	}
};
template <int... II, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, SlotRet (Obj::*)(SlotArgs...) const> {
	static void call(SlotRet (Obj::*f)(SlotArgs...) const, Obj *o, void **arg) {
		(o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II+1]))...), ApplyReturnValue<R>(arg[0]);
	}
};
```

```cpp
template <int...> struct IndexesList {};
template <typename IndexList, int Right> struct IndexesAppend;
template <int... Left, int Right> struct IndexesAppend<IndexesList<Left...>, Right>
{ typedef IndexesList<Left..., Right> Value; };
template <int N> struct Indexes
{ typedef typename IndexesAppend<typename Indexes<N - 1>::Value, N - 1>::Value Value; };
template <> struct Indexes<0> { typedef IndexesList<> Value; };
template<typename Func> struct FunctionPointer { enum {ArgumentCount = -1, IsPointerToMemberFunction = false}; };
```
### 4.2 VS的实现
```cpp

template<bool,
	class _Ty1,
	class _Ty2>
	struct _If
	{	// type is _Ty2 for assumed false
	typedef _Ty2 type;
	};

template<class _Ty1,
	class _Ty2>
	struct _If<true, _Ty1, _Ty2>
	{	// type is _Ty1 for assumed true
	typedef _Ty1 type;
	};

struct _Invoker_pmf_object
	{	// INVOKE a pointer to member function on an object
	template<class _Decayed,
		class _Ty1,
		class... _Types2>
		static auto _Call(_Decayed _Pmf, _Ty1&& _Arg1, _Types2&&... _Args2)
		-> decltype((_STD forward<_Ty1>(_Arg1).*_Pmf)(
			_STD forward<_Types2>(_Args2)...))
		{	// INVOKE a pointer to member function on an object
		return ((_STD forward<_Ty1>(_Arg1).*_Pmf)(
			_STD forward<_Types2>(_Args2)...));
		}
	};

struct _Invoker_pmf_pointer
	{	// INVOKE a pointer to member function on a [smart] pointer
	template<class _Decayed,
		class _Ty1,
		class... _Types2>
		static auto _Call(_Decayed _Pmf, _Ty1&& _Arg1, _Types2&&... _Args2)
		-> decltype(((*_STD forward<_Ty1>(_Arg1)).*_Pmf)(
			_STD forward<_Types2>(_Args2)...))
		{	// INVOKE a pointer to member function on a [smart] pointer
		return (((*_STD forward<_Ty1>(_Arg1)).*_Pmf)(
			_STD forward<_Types2>(_Args2)...));
		}
	};

struct _Invoker_pmd_object
	{	// INVOKE a pointer to member data on an object
	template<class _Decayed,
		class _Ty1>
		static auto _Call(_Decayed _Pmd, _Ty1&& _Arg1)
		-> decltype(_STD forward<_Ty1>(_Arg1).*_Pmd)
		{	// INVOKE a pointer to member data on an object
		return (_STD forward<_Ty1>(_Arg1).*_Pmd);
		}
	};

struct _Invoker_pmd_pointer
	{	// INVOKE a pointer to member data on a [smart] pointer
	template<class _Decayed,
		class _Ty1>
		static auto _Call(_Decayed _Pmd, _Ty1&& _Arg1)
		-> decltype((*_STD forward<_Ty1>(_Arg1)).*_Pmd)
		{	// INVOKE a pointer to member data on a [smart] pointer
		return ((*_STD forward<_Ty1>(_Arg1)).*_Pmd);
		}
	};

struct _Invoker_functor
	{	// INVOKE a function object
	template<class _Callable,
		class... _Types>
		static auto _Call(_Callable&& _Obj, _Types&&... _Args)
		-> decltype(_STD forward<_Callable>(_Obj)(
			_STD forward<_Types>(_Args)...))
		{	// INVOKE a function object
		return (_STD forward<_Callable>(_Obj)(
			_STD forward<_Types>(_Args)...));
		}
	};

template<class _Callable,
	class _Ty1,
	class _Decayed = typename decay<_Callable>::type,
	bool _Is_pmf = is_member_function_pointer<_Decayed>::value,
	bool _Is_pmd = is_member_object_pointer<_Decayed>::value>
	struct _Invoker1;

template<class _Callable,
	class _Ty1,
	class _Decayed>
	struct _Invoker1<_Callable, _Ty1, _Decayed, true, false>
		: _If<is_base_of<
			typename _Is_memfunptr<_Decayed>::_Class_type,
			typename decay<_Ty1>::type>::value,
		_Invoker_pmf_object,
		_Invoker_pmf_pointer>::type
	{	// pointer to member function
	};

template<class _Callable,
	class _Ty1,
	class _Decayed>
	struct _Invoker1<_Callable, _Ty1, _Decayed, false, true>
		: _If<is_base_of<
			typename _Is_member_object_pointer<_Decayed>::_Class_type,
			typename decay<_Ty1>::type>::value,
		_Invoker_pmd_object,
		_Invoker_pmd_pointer>::type
	{	// pointer to member data
	};

template<class _Callable,
	class _Ty1,
	class _Decayed>
	struct _Invoker1<_Callable, _Ty1, _Decayed, false, false>
		: _Invoker_functor
	{	// function object
	};

template<class _Callable,
	class... _Types>
	struct _Invoker;

template<class _Callable>
	struct _Invoker<_Callable>
		: _Invoker_functor
	{	// zero arguments
	};

template<class _Callable,
	class _Ty1,
	class... _Types2>
	struct _Invoker<_Callable, _Ty1, _Types2...>
		: _Invoker1<_Callable, _Ty1>
	{	// one or more arguments
	};

template<class _Callable,
	class... _Types> inline
	auto invoke(_Callable&& _Obj, _Types&&... _Args)
	-> decltype(_Invoker<_Callable, _Types...>::_Call(
		_STD forward<_Callable>(_Obj), _STD forward<_Types>(_Args)...))
	{	// INVOKE a callable object
	return (_Invoker<_Callable, _Types...>::_Call(
		_STD forward<_Callable>(_Obj), _STD forward<_Types>(_Args)...));
	}

template<class _Rx,
	bool = is_void<_Rx>::value>
	struct _Forced
	{	// tag to give INVOKE an explicit return type
	};

struct _Unforced
	{	// tag to distinguish bind() from bind<R>()
	};

template<class _Cv_void,
	class... _Valtys> inline
	void _Invoke_ret(_Forced<_Cv_void, true>, _Valtys&&... _Vals)
	{	// INVOKE, "implicitly" converted to void
	_STD invoke(_STD forward<_Valtys>(_Vals)...);
	}

template<class _Rx,
	class... _Valtys> inline
	_Rx _Invoke_ret(_Forced<_Rx, false>, _Valtys&&... _Vals)
	{	// INVOKE, implicitly converted to _Rx
	return (_STD invoke(_STD forward<_Valtys>(_Vals)...));
	}

template<class... _Valtys> inline
	auto _Invoke_ret(_Forced<_Unforced, false>, _Valtys&&... _Vals)
	-> decltype(_STD invoke(_STD forward<_Valtys>(_Vals)...))
	{	// INVOKE, unchanged
	return (_STD invoke(_STD forward<_Valtys>(_Vals)...));
	}

	// TEMPLATE CLASS result_of
struct _Unique_tag_result_of
	{	// used by workaround below
	};

template<class _Void,
	class... _Types>
	struct _Result_of
	{	// selected when _Fty isn't callable with _Args
	};

template<class... _Types>
	struct _Result_of<
		void_t<
			_Unique_tag_result_of,	// TRANSITION, C1XX
			decltype(_STD invoke(_STD declval<_Types>()...))>,
		_Types...>
	{	// selected when _Fty is callable with _Args
	typedef decltype(_STD invoke(_STD declval<_Types>()...)) type;
	};

template<class _Fty>
	struct result_of
	{	// explain usage
	static_assert(_Always_false<_Fty>::value,
		"result_of<CallableType> is invalid; use "
		"result_of<CallableType(zero or more argument types)> instead.");
	};

#define _RESULT_OF(CALL_OPT, X1, X2) \
template<class _Fty, \
	class... _Args> \
	struct result_of<_Fty CALL_OPT (_Args...)> \
		: _Result_of<void, _Fty, _Args...> \
	{	/* template to determine result of call operation */ \
	};

_NON_MEMBER_CALL(_RESULT_OF, , )
#undef _RESULT_OF

	// TEMPLATE STRUCT _Weak_types
template<class _Ty,
	class = void>
	struct _Weak_result_type
	{	// default definition
	};

template<class _Ty>
	struct _Weak_result_type<_Ty, void_t<
		typename _Ty::result_type> >
	{	// defined if _Ty::result_type exists
	typedef typename _Ty::result_type result_type;
	};

template<class _Ty,
	class = void>
	struct _Weak_argument_type
		: _Weak_result_type<_Ty>
	{	// default definition
	};

template<class _Ty>
	struct _Weak_argument_type<_Ty, void_t<
		typename _Ty::argument_type> >
		: _Weak_result_type<_Ty>
	{	// defined if _Ty::argument_type exists
	typedef typename _Ty::argument_type argument_type;
	};

template<class _Ty,
	class = void>
	struct _Weak_binary_args
		: _Weak_argument_type<_Ty>
	{	// default definition
	};

template<class _Ty>
	struct _Weak_binary_args<_Ty, void_t<
		typename _Ty::first_argument_type,
		typename _Ty::second_argument_type> >
		: _Weak_argument_type<_Ty>
	{	// defined if both types exist
	typedef typename _Ty::first_argument_type first_argument_type;
	typedef typename _Ty::second_argument_type second_argument_type;
	};

template<class _Ty>
	struct _Weak_types
	{	// provide nested types (sometimes)
	typedef _Is_function<typename remove_pointer<_Ty>::type> _Is_f_or_pf;
	typedef _Is_memfunptr<typename remove_cv<_Ty>::type> _Is_pmf;
	typedef typename _If<_Is_f_or_pf::_Bool_type::value, _Is_f_or_pf,
		typename _If<_Is_pmf::_Bool_type::value, _Is_pmf,
		_Weak_binary_args<_Ty> >::type>::type type;
	};

	// TEMPLATE CLASS reference_wrapper
template<class _Ty>
	class reference_wrapper
		: public _Weak_types<_Ty>::type
	{	// stand-in for an assignable reference
public:
	static_assert(is_object<_Ty>::value || is_function<_Ty>::value,
		"reference_wrapper<T> requires T to be an object type "
		"or a function type.");

	typedef _Ty type;

	reference_wrapper(_Ty& _Val) _NOEXCEPT
		: _Ptr(_STD addressof(_Val))
		{	// construct
		}

	operator _Ty&() const _NOEXCEPT
		{	// return reference
		return (*_Ptr);
		}

	_Ty& get() const _NOEXCEPT
		{	// return reference
		return (*_Ptr);
		}

	template<class... _Types>
		auto operator()(_Types&&... _Args) const
		-> decltype(_STD invoke(get(), _STD forward<_Types>(_Args)...))
		{	// invoke object/function
		return (_STD invoke(get(), _STD forward<_Types>(_Args)...));
		}

	reference_wrapper(_Ty&&) = delete;

private:
	_Ty *_Ptr;
	};

	// TEMPLATE FUNCTIONS ref AND cref
template<class _Ty> inline
	reference_wrapper<_Ty>
		ref(_Ty& _Val) _NOEXCEPT
	{	// create reference_wrapper<_Ty> object
	return (reference_wrapper<_Ty>(_Val));
	}

template<class _Ty>
	void ref(const _Ty&&) = delete;

template<class _Ty> inline
	reference_wrapper<_Ty>
		ref(reference_wrapper<_Ty> _Val) _NOEXCEPT
	{	// create reference_wrapper<_Ty> object
	return (_STD ref(_Val.get()));
	}

template<class _Ty> inline
	reference_wrapper<const _Ty>
		cref(const _Ty& _Val) _NOEXCEPT
	{	// create reference_wrapper<const _Ty> object
	return (reference_wrapper<const _Ty>(_Val));
	}

template<class _Ty>
	void cref(const _Ty&&) = delete;

template<class _Ty> inline
	reference_wrapper<const _Ty>
		cref(reference_wrapper<_Ty> _Val) _NOEXCEPT
	{	// create reference_wrapper<const _Ty> object
	return (_STD cref(_Val.get()));
	}

	// TEMPLATE CLASS _Unrefwrap
template<class _Ty>
	struct _Unrefwrap_helper
	{	// leave unchanged if not a reference_wrapper
	typedef _Ty type;
	static constexpr bool _Is_refwrap = false;
	};

template<class _Ty>
	struct _Unrefwrap_helper<reference_wrapper<_Ty> >
	{	// make a reference from a reference_wrapper
	typedef _Ty& type;
	static constexpr bool _Is_refwrap = true;
	};

template<class _Ty>
	struct _Unrefwrap
	{	// decay, then unwrap a reference_wrapper
	typedef typename decay<_Ty>::type _Ty1;
	typedef typename _Unrefwrap_helper<_Ty1>::type type;
	static constexpr bool _Is_refwrap = _Unrefwrap_helper<_Ty1>::_Is_refwrap;
	};
```

### 4.3 GCC的实现
```cpp

 /// is_function
  template<typename>
    struct is_function
    : public false_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes...) _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes...) & _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes...) && _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes......) _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes......) & _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes......) && _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes...) const _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes...) const & _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes...) const && _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes......) const _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes......) const & _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes......) const && _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes...) volatile _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes...) volatile & _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes...) volatile && _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes......) volatile _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes......) volatile & _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes......) volatile && _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes...) const volatile _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes...) const volatile & _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes...) const volatile && _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes......) const volatile _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes......) const volatile & _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct is_function<_Res(_ArgTypes......) const volatile && _GLIBCXX_NOEXCEPT_QUAL>
    : public true_type { };


  template<typename>
    struct __is_member_object_pointer_helper
    : public false_type { };

  template<typename _Tp, typename _Cp>
    struct __is_member_object_pointer_helper<_Tp _Cp::*>
    : public integral_constant<bool, !is_function<_Tp>::value> { };

  /// is_member_object_pointer
  template<typename _Tp>
    struct is_member_object_pointer
    : public __is_member_object_pointer_helper<
				typename remove_cv<_Tp>::type>::type
    { };

	template<bool, bool, typename _Functor, typename... _ArgTypes>
    struct __result_of_impl
    {
      typedef __failure_type type;
    };

  template<typename _MemPtr, typename _Arg>
    struct __result_of_impl<true, false, _MemPtr, _Arg>
    : public __result_of_memobj<typename decay<_MemPtr>::type,
				typename __inv_unwrap<_Arg>::type>
    { };

  template<typename _MemPtr, typename _Arg, typename... _Args>
    struct __result_of_impl<false, true, _MemPtr, _Arg, _Args...>
    : public __result_of_memfun<typename decay<_MemPtr>::type,
				typename __inv_unwrap<_Arg>::type, _Args...>
    { };

  // [func.require] paragraph 1 bullet 5:
  struct __result_of_other_impl
  {
    template<typename _Fn, typename... _Args>
      static __result_of_success<decltype(
      std::declval<_Fn>()(std::declval<_Args>()...)
      ), __invoke_other> _S_test(int);

    template<typename...>
      static __failure_type _S_test(...);
  };

   template<typename>
    struct __is_member_object_pointer_helper
    : public false_type { };

  template<typename _Tp, typename _Cp>
    struct __is_member_object_pointer_helper<_Tp _Cp::*>
    : public integral_constant<bool, !is_function<_Tp>::value> { };

  /// is_member_object_pointer
  template<typename _Tp>
    struct is_member_object_pointer
    : public __is_member_object_pointer_helper<
				typename remove_cv<_Tp>::type>::type
    { };


  template<typename>
    struct __is_member_function_pointer_helper
    : public false_type { };

  template<typename _Tp, typename _Cp>
    struct __is_member_function_pointer_helper<_Tp _Cp::*>
    : public integral_constant<bool, is_function<_Tp>::value> { };

  /// is_member_function_pointer
  template<typename _Tp>
    struct is_member_function_pointer
    : public __is_member_function_pointer_helper<
				typename remove_cv<_Tp>::type>::type
    { };

  template<typename _Functor, typename... _ArgTypes>
    struct __result_of_impl<false, false, _Functor, _ArgTypes...>
    : private __result_of_other_impl
    {
      typedef decltype(_S_test<_Functor, _ArgTypes...>(0)) type;
    };

  // __invoke_result (std::invoke_result for C++11)
  template<typename _Functor, typename... _ArgTypes>
    struct __invoke_result
    : public __result_of_impl<
        is_member_object_pointer<
          typename remove_reference<_Functor>::type
        >::value,
        is_member_function_pointer<
          typename remove_reference<_Functor>::type
        >::value,
	_Functor, _ArgTypes...
      >::type
    { };

  template<typename _Functor, typename... _ArgTypes>
    struct result_of<_Functor(_ArgTypes...)>
    : public __invoke_result<_Functor, _ArgTypes...>
    { };


  // __invoke_result (std::invoke_result for C++11)
  template<typename _Functor, typename... _ArgTypes>
    struct __invoke_result
    : public __result_of_impl<
        is_member_object_pointer<
          typename remove_reference<_Functor>::type
        >::value,
        is_member_function_pointer<
          typename remove_reference<_Functor>::type
        >::value,
	_Functor, _ArgTypes...
      >::type
    { };

  template<typename _Functor, typename... _ArgTypes>
    struct result_of<_Functor(_ArgTypes...)>
    : public __invoke_result<_Functor, _ArgTypes...>
    { };

  template<typename _Tp, typename _Up = typename __inv_unwrap<_Tp>::type>
    constexpr _Up&&
    __invfwd(typename remove_reference<_Tp>::type& __t) noexcept
    { return static_cast<_Up&&>(__t); }

  template<typename _Res, typename _Fn, typename... _Args>
    constexpr _Res
    __invoke_impl(__invoke_other, _Fn&& __f, _Args&&... __args)
    { return std::forward<_Fn>(__f)(std::forward<_Args>(__args)...); }

  template<typename _Res, typename _MemFun, typename _Tp, typename... _Args>
    constexpr _Res
    __invoke_impl(__invoke_memfun_ref, _MemFun&& __f, _Tp&& __t,
		  _Args&&... __args)
    { return (__invfwd<_Tp>(__t).*__f)(std::forward<_Args>(__args)...); }

  template<typename _Res, typename _MemFun, typename _Tp, typename... _Args>
    constexpr _Res
    __invoke_impl(__invoke_memfun_deref, _MemFun&& __f, _Tp&& __t,
		  _Args&&... __args)
    {
      return ((*std::forward<_Tp>(__t)).*__f)(std::forward<_Args>(__args)...);
    }

  template<typename _Res, typename _MemPtr, typename _Tp>
    constexpr _Res
    __invoke_impl(__invoke_memobj_ref, _MemPtr&& __f, _Tp&& __t)
    { return __invfwd<_Tp>(__t).*__f; }

  template<typename _Res, typename _MemPtr, typename _Tp>
    constexpr _Res
    __invoke_impl(__invoke_memobj_deref, _MemPtr&& __f, _Tp&& __t)
    { return (*std::forward<_Tp>(__t)).*__f; }

  /// Invoke a callable object.
  template<typename _Callable, typename... _Args>
    constexpr typename __invoke_result<_Callable, _Args...>::type
    __invoke(_Callable&& __fn, _Args&&... __args)
    noexcept(__is_nothrow_invocable<_Callable, _Args...>::value)
    {
      using __result = __invoke_result<_Callable, _Args...>;
      using __type = typename __result::type;
      using __tag = typename __result::__invoke_type;
      return std::__invoke_impl<__type>(__tag{}, std::forward<_Callable>(__fn),
					std::forward<_Args>(__args)...);
    }

  /// Invoke a callable object.
  template<typename _Callable, typename... _Args>
    inline invoke_result_t<_Callable, _Args...>
    invoke(_Callable&& __fn, _Args&&... __args)
    noexcept(is_nothrow_invocable_v<_Callable, _Args...>)
    {
      return std::__invoke(std::forward<_Callable>(__fn),
			   std::forward<_Args>(__args)...);
    }

```
