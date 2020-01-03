= 漏洞扫描器 =
AUTHOR: liushuo AT conew.com

== bin目录文件列表 ==
  ksafevul.exe            提供可视化的漏洞修复功能
  ksafevul.dll              漏洞修复引擎
  leakscan_test.exe   命令行版本例子程序
  data\*.*                   漏洞库目录
         office.dat          微软office 32位软件补丁
         office64.dat       微软office 64位软件补丁
         system.dat       windows 32位补丁
         system64.dat   window 64位补丁
         soft.dat             第三方软件补丁

== 漏洞修复sdk 功能 =
1. 漏洞的快速扫描
    主要是坚持系统是否存在补丁，一旦发现补丁就快速返回；
    另外针对x64 位系统只能通过接口来获取补丁的做了速度优化；
2. 漏洞全盘扫描
    获取全部补丁，以及下载等
3. 漏洞的修复
4. 漏洞库升级
    需要knup.dll

== sdk 例子 ==
编译Release, 生成同一目录的leakscan_test.exe 和 ksafevul.dll , 在目录的子目录里面Lib 文件里面是漏洞库文件
Data\
 office.dat office 库
 system.dat 系统库
 system64.dat 64 位系统库
 soft.dat 软件库
直接运行 leakscan_test.exe 就可以看到效果

== sdk 使用方法 ==
1. 快速扫描
参考例子 TestQuickScan()
HRESULT WINAPI ExpressScanLeak(IVulCollection **pVulCollection, DWORD dwFlags, BOOL bQuickScan);
如果成功, 先调用 IVulCollection::get_Status
 如果S_OK, 则根据status 来判断是否有漏洞风险, -1, 0 表示可能无风险, 1 表示可能有风险
 否则,根据 IVulCollection::get_VulCount 和 IVulCollection::get_Item 来判断是否有漏洞存在, 具体看例子中函数 DumpIVulCollection 

2. 漏洞全盘扫描和修复
参考例子 TestRepair()
使用接口 IVulfix 的方法 Scan 扫描漏洞, 扫描完成后使用方法 GetResults, GetSoftVuls 分别获取系统漏洞列表, 和软件漏洞列表
使用接口 IRepairVul 修复漏洞



= INSTALL =
 
编译：
    编译环境vc2005 sp1 + vista sdk
    注意：vista sdk安装完毕之后，请在vc工具->项目与解决方案中->vc++ directories中的
          executable files、include files、library files中将vista sdk相关目录添加并提至第一位。
 

