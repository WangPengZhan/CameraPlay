# 单例类跨dll的问题

## 问题的发现

在调试过程中发现单例的线程池在各个dll都存在
[![实现](http://10.1.65.34/group1/M00/22/58/CgFBImOHOKyAH2oUAAEoPhQh7Ew362.png)](http://10.1.65.34/group1/M00/22/58/CgFBImOHOKyAH2oUAAEoPhQh7Ew362.png)

其具体实现为：

[![](http://10.1.65.34/group1/M00/22/58/CgFBImOHOPiAS9VaAAAhpxboqX8507.png)](http://10.1.65.34/group1/M00/22/58/CgFBImOHOPiAS9VaAAAhpxboqX8507.png)

这里是使用了HPR的单例模板简化代码：

```cpp
template<typename T>
class singleton
{
public:
	static inline T* GetInstance()
	{
		if (instance_ == 0)
		{
			lock_.lock();
			if (instance_ == 0)
			{
				instance_ = new T;
			}
			lock_.unlock();
		}
		return instance_;
	}

	static inline void FreeInstance()
	{
		free_ = true;
		lock_.lock();
		if (instance_ != 0)
		{
			delete instance_;
			instance_ = 0;
		}
		lock_.unlock();
	}

protected:
	singleton() {}
	virtual ~singleton()
	{ 
		if (!free_)
		{
			lock_.lock();
			if (instance_ != 0)
			{
				delete instance_;
				instance_ = 0;
			}
			lock_.unlock();
		}
	}
private:
	singleton(const singleton&) {}
	singleton& operator=(const singleton&) {}

private:
	static T* instance_;
	static hpr::hpr_mutex lock_;
	static bool free_;
};
```
这里单例的实现其实是有很大问题的 但是这里不作讨论。

## 原因分析
static变量是单个最终成果物单元(动态库 静态库[包含代码和符号的] 可执行文件)的静态变量，内存将在程序运行和库加载时被分配相关内存中，此时如果你编可执行文件的代码和编动态库的代码都包含了上述的单例头文件，那么这个“单例”将会出现两个实例(即分配两次)，一个实例的内存在可执行文件中，一个实例的内存在动态链接库中，在运行存在于可执行文件中的代码段时，使用的是可执行文件中的那一份“单例”，而在运行存在于动态链接库中的代码段时，使用的则是动态链接库中的那一份“单例”，这其实本质就是所谓的动态链接库间的内存隔离导致的问题。

其中动态库 静态库(包含代码和符号的) 可执行文件等我这里称之为最终成果物单元(自己起的)


## 解决方法
解决这个问题有三种方法：
1.设计代码时选好单例的内存该放在哪，通过导入导出符号解决问题(这里更推荐C++整个类导出)
2.搞个统一的地方大家的单例共同注册到一起(单独的dll)，用的时候拿出来。
3.写一个管理单例类型的类(使用标准库的类型信息：std::type_index std::type_info)

## 总结
1.编译生成规则和动态库加载规则要熟记
2.接口化编程隔离内部实现
