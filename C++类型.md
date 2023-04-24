# C++之类型转化
C++类型转换主要分为两种:
* 隐式类型转换
* 显式(强制)类型转换

## 隐式转化
隐式类型转换——编译器默认进行的类型转换行为。
主要分为两种：
* 内置数据类型
* 自定义数据类型

### 内置数据类型
规则：
* 运算类：转换为表达式中的最大宽度的类型
* 赋值类：转换为目标类型

主要是以下情况：
1. 算术运算，如int + short + double 时，int和short的变量会隐式转换为double，再与double进行加法。
2. 关系运算，如if ( int < unsigned int)，会转换为较宽类型的unsigned int
3. 赋值运算，如int = double，double的变量将会被截断成int，然后赋值给左边
4. 函数返回，如fun()返回值为double，函数实现中，却return int，此时会将int隐式转换为double返回。
5. 函数返回后，如int = fun()，会将fun()返回的double类型转换为int。
6. 函数传参, 如void func(double dArg); func(1); 会将整形1隐式转化为double类型
注释：
函数传参时会涉及函数匹配原则：按照最佳匹配原则。其中有一条就是类型隐式转化后匹配

### 自定义数据类型
根据自定义数据类型的构造函数或者类型操作符重载
1. 构造函数
多个构造函数重载
如：
``` cpp
class Type
{
public:
    Type(int n) {};
    Type(const char* p) {}; 
};

void main()
{
    // 显式构造
    Type s1("China");
    Type s2 = Type("China"); 
    Type s3(10); 
    Type s4 = Type(10); 

    // 隐式转化
    Type s4 = 10; // OK，隐式转换
    Type s5 = 'A'; // OK，隐式转换
}
```
2. 类型操作符 赋值操作符重载
operator bool() operator Type() TypeTo operator =(const TypeFrom& v)等类型操作符重载
``` cpp
class TypeA
{
public:
    Type(int n) {};
    Type(const char* p) {}; 
};

class TypeB
{
public:
    operator Type1();
    TypeB& operator=(const TypeA& v);
    operator bool();
}

void main()
{
    TypeA typeA1;

    // 隐式转化
    TypeB typeB1 = typeA1;
    bool isTypeB = typeB1;
    TypeB typeB2(typeA1);
}
```
注释：
* 风险一般存在于自定义类型转换
* 此种构造函数转换，可以在构造函数前加关键字explicit
* 类型操作符和赋值操作符一般用于不修改要转化类的内容，即可达到构造函数的效果

## 显式转化
显式转化主要分为两种：旧式显式转化和新式显式转化

### 旧式显式转化
``` cpp
type(expr); // 函数形式的强制类型转换
(type)expr; // C语言风格的强制类型转换
```

### 新式显式转化(现代C++风格的类型转换,更推荐)
``` cpp
cast_name<type>(expression)
```
* type是转换的目标类型
* expression是被转换的值
* cast_name有static_cast，dynamic_cast，const_cast和reinterpret_cast四种

目的：规范C中的类型转换，加强类型转换的可视性

#### static_cast
能够明确的类型转换都可以使用static_cast，但不能转换掉底层const，volatile和__unaligned属性。由于不提供运行时的检查，所以叫static_cast。因此，需要在编写程序时确认转换的安全性。

场景：
* 用于类层次结构中，父类和子类之间指针和引用的转换；
* 用于基本数据类型之间的转换，例如把int转char，int转enum等，需要编写程序时来确认安全性；
* 把void指针转换成目标类型的指针（这是极其不安全的）；

注释：
* 可以代替旧式显式转化

#### dynamic_cast
dynamic_cast会在运行时检查类型转换是否合法，具有一定的安全性。由于运行时的检查，所以会额外消耗一些性能。dynamic_cast转换仅适用于指针或引用。在转换可能发生的前提下，dynamic_cast会尝试转换，若指针转换失败，则返回空指针，若引用转换失败，则抛出异常。

场景：
* 继承中的向下转换(更安全)
* void*的转换(更安全)
* 菱形继承中的上行转换需要指定路径

注释：
* 运行时检查类型
* 仅适用于指针或引用
* 若指针转换失败，则返回空指针，若引用转换失败，则抛出异常。

#### const_cast
const_cast用于移除类型的const、volatile和__unaligned属性。

#### reinterpret_cast
编译期完成，可以转换任何类型的指针，所以极不安全。非极端情况不要使用。

场景：
* 重新解释二进制的底层操作
