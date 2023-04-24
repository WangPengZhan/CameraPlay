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
