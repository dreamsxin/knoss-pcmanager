#pragma once

//////////////////////////////////////////////////////////////////////////
// CONTROL ID

#define IDC_BTN_SYS_CLOSE                   60001
#define IDC_BTN_SYS_MAX                     60002
#define IDC_BTN_SYS_MIN                     60003
#define IDC_LBL_MSGBOX_TITLE                60004
#define IDC_ICON_MSGBOX_ICON                60005
#define IDC_LBL_MSGBOX_TEXT                 60006
#define IDC_CHK_MSGBOX_NONOTIFYLATER        60007
#define IDC_DIV_MSGBOX_BUTTONS              60008
#define IDC_DIV_MSGBOX_CONTENT              60009
#define IDC_CHK_MSGBOX_NEVERSHOW            60010

//////////////////////////////////////////////////////////////////////////
// 4000-4999 系统优化，开机启动项优化

#define IDC_TAB_SYSOPT_LEFT						4001
#define IDC_DIV_SYSOPT_RUN						4002
#define IDC_LST_RUNOPT							4003
#define IDC_TAB_RUNOPT_TYPE						4004
//4008,4009
#define IDC_CHECKBOX_RUNOPT_HIDE_SYSTEM			4005
#define IDC_DIV_RUNOPT_CONTROL_SYS				4010
#define IDC_LBL_RUNOPT_REFRESH					4011
#define IDC_BTN_RUNOPT_CANCEL_ALL				4012
#define IDC_DIV_RUNOPT_BOTTOM					4013
#define IDC_TABPAGE_RUNOPT_START				4014
#define IDC_TABPAGE_RUNOPT_SERVICE				4015
#define IDC_TABPAGE_RUNOPT_TASK					4016
#define IDC_DIV_SYSOPT_MORE						4017
#define IDC_BTN_SYSOPT_WAIT_MORE				4018
#define IDC_DIV_RUNOPT_TOP_RUN					4019
#define IDC_DIV_RUNOPT_TOP_SERVICE				4020
#define IDC_DIV_RUNOPT_TOP_TASK					4021
#define IDC_LBL_RUNOPT_TOP_MORE					4022
#define IDC_DIV_RUNOPT_BTM_RUN_JOB_OP			4023
#define IDC_LBL_RUNOPT_BTM_SERVICE_OP			4024

//
#define IDC_CHECKBOX_SHOW_DISABLE				4025
#define IDC_DIV_SYSOPT_CLR_HISTORY				4028
#define IDC_DIV_CLR_NO_DOWN						4029
#define IDC_DIV_CLR_DOWNING						4030
#define IDC_DIV_CLR_INSTALLED					4031
#define IDC_TXT_CLR_NO_DOWN						4032
//#define IDC_BTN_CLR_DOWN_PACK					4033
#define IDC_TXT_CLR_NO_DOWN_TOP_TIP				4034
#define IDC_ICON_CLR_NO_DOWN_TOP_TIP			4035
#define IDC_TXT_CLR_UPDATE						4036
#define IDC_TXT_CLR_UPDATEED					4037
#define IDC_DIV_ENABLE_SYSOPT					4038
#define IDC_DIV_UPDATE_SYSOPT					4039
//#define IDC_BTN_UPDATE_SYSOPT					4040
//#define IDC_TXT_RUN_OLD_SYSOPT				4041
#define IDC_DIV_CLR_STATE						4042
#define IDC_DIV_CLR_UPDATE_STATE				4043
#define IDC_TXT_UPDATE_FEATURE1					4044
#define IDC_TXT_UPDATE_FEATURE2					4045
#define IDC_TXT_UPDATE_FEATURE3					4046
#define IDC_TXT_UPDATE_FEATURE4					4047
#define IDC_LNK_SYSOPT_SETTING					4048	//开机提示设置

#define IDC_LST_RUN_AUTOOPT						4054
#define IDC_DIV_AUTOOPT							4055
#define IDC_DIV_RUN_TOPTIP						4060

#define IDC_IMG_AUTOOPT_TOP_DOING				4063
#define IDC_DIV_AUTOOPT_BTM						4006
//{
#define IDC_DIV_AUTOOPT_NO_OPTITEM				4059
#define IDC_LNK_VIEW_RUN						4064//查看开机启动项
#define IDC_LNK_VIEW_SERV						4065//查看开机加载服务项
#define IDC_LNK_VIEW_TASK						4066//查看计划任务项
#define IDC_LNK_VIEW_IGNORED					4007//查看已忽略的优化项
//}
//{
#define IDC_DIV_NOTASK_INFO						4069
#define IDC_LNK_TRASHCLEANER					4070//清理垃圾
#define IDC_LNK_ZHUANGJIBIBEI					4071//装机必备
//}
//{一键优化顶部提示信息
#define IDC_DIV_AUTOOPT_TOP						4084
#define IDC_TXT_AUTOOPT_TOP_DETAIL				4058
#define IDC_DIV_AUTOOPT_TOP_INIT				4056//一键优化初始信息
#define IDC_DIV_AUTOOPT_TOP_DOING				4073//正在优化
#define IDC_DIV_AUTOOPT_TOP_DONE				4074//优化完毕
#define IDC_DIV_AUTOOPT_TOP_DONE_PART			4075//部分优化完毕
#define IDC_TXT_AUTOOPT_TOTAL_NUM				4076
#define IDC_TXT_AUTOOPT_OPTED_NUM				4077
#define IDC_TXT_AUTOOPT_NOOPTED_NUM				4078
#define IDC_TXT_AUTOOPT_TOTAL_DONE_NUM			4080
#define IDC_TXT_AUTOOPT_AFTER_TOTAL_NUM			4079
#define IDC_TXT_AUTOOPT_AFTER_OPTED_NUM			4081
#define IDC_TXT_AUTOOPT_AFTER_NOOPTED_NUM		4082
#define IDC_TXT_AUTOOPT_AFTER_TOTALDONE_NUM		4083
//}
//“一键优化”底部
//{
#define IDC_DIV_AUTOOPT_BTM_HAVE				4026
#define IDC_CHECKBOX_AUTOOPT_ALL				4027//全选
#define IDC_LNK_ADDTO_IGNORED					4086//添加到忽略
#define IDC_BTN_RUN_AUTOOPT_DO					4053//立即优化
//}
//{
#define IDC_DIV_AUTOOPT_BTM_CONTINUE			4061
#define IDC_LNK_AUTOOPT_BTM_REFRESH				4062//刷新
#define IDC_BTN_AUTOOPT_BTM_CONTINUE			4072//继续优化
#define IDC_CHECKBOX_AUTOOPT_ALL2				4057//全选
#define IDC_LNK_AUTOOPT_CANCELALL				4067//撤销所有
#define IDC_LNK_ADDTO_IGNORED2					4087//添加到忽略2
//}

#define IDC_LNK_SHOW_IGNORED					4085//查看“已忽略”的优化项
#define IDC_TABPAGE_RUNOPT_ONEKEY_OPT			4100//新增一键优化Tab页

//////////////////////////////////////////////////////////////////////////
//开机提示设置窗口
#define IDC_BTN_SETTINGS_OK						1
#define IDC_BTN_SETTINGS_CANCEL					2
#define IDC_RADIO_CONDITIONS_SHOW				3
#define IDC_CHECK_AFTER_OPT_START				4
#define IDC_CHECK_BOOT_TOOSLOW					5
#define IDC_RADIO_ALLWAYS_SHOW					6
#define IDC_RADIO_NEVER_SHOW					7

//////////////////////////////////////////////////////////////////////////
//一键优化忽略
#define IDC_DIV_IGNORE_LIST						2
#define IDC_LST_IGNORE_LIST						3
#define IDC_CHECK_IGNORE_CHECKALL				4
#define IDC_BTN_CANCEL_IGNORE					5
#define IDC_BTN_CLOSE							6
#define IDC_DIV_NO_IGNOREDITEM					7

// STRING ID

#define IDS_APP_FONT                            0
#define IDS_APP_NAME                            1
#define IDS_MSGBOX_BUTTON_XML                   17
#define IDS_MSGBOX_OK                           18
#define IDS_MSGBOX_CANCEL                       19
#define IDS_MSGBOX_ABORT                        20
#define IDS_MSGBOX_RETRY                        21
#define IDS_MSGBOX_IGNORE                       22
#define IDS_MSGBOX_YES                          23
#define IDS_MSGBOX_NO                           24
#define IDS_MSGBOX_CONTINUE                     25
#define IDS_MSGBOX_BUTTON_DIV                   26

#define IDS_SYSOPT_4000                         4000
#define IDS_SYSOPT_4001                         4001
#define IDS_SYSOPT_4002                         4002
#define IDS_SYSOPT_4003                         4003
#define IDS_SYSOPT_4004                         4004
#define IDS_SYSOPT_4005                         4005
#define IDS_SYSOPT_4006                         4006
#define IDS_SYSOPT_4007                         4007
#define IDS_SYSOPT_4008                         4008
#define IDS_SYSOPT_4009                         4009
#define IDS_SYSOPT_4010                         4010
#define IDS_SYSOPT_4011                         4011
#define IDS_SYSOPT_4012                         4012
#define IDS_SYSOPT_4013                         4013
#define IDS_SYSOPT_4014                         4014
#define IDS_SYSOPT_4015                         4015
#define IDS_SYSOPT_4016                         4016
#define IDS_SYSOPT_4017                         4017
#define IDS_SYSOPT_4018                         4018
#define IDS_SYSOPT_4019                         4019
#define IDS_SYSOPT_4020                         4020
#define IDS_SYSOPT_4021                         4021
#define IDS_SYSOPT_4022                         4022
#define IDS_SYSOPT_4023                         4023
#define IDS_SYSOPT_4024                         4024
#define IDS_SYSOPT_4025                         4025
#define IDS_SYSOPT_4026                         4026
#define IDS_SYSOPT_4027                         4027
#define IDS_SYSOPT_4028                         4028
#define IDS_SYSOPT_4029                         4029
#define IDS_SYSOPT_4030                         4030
#define IDS_SYSOPT_4031                         4031
#define IDS_SYSOPT_4032                         4032
#define IDS_SYSOPT_4033                         4033
#define IDS_SYSOPT_4034                         4034
#define IDS_SYSOPT_4035                         4035
#define IDS_SYSOPT_4036                         4036
#define IDS_SYSOPT_4037                         4037
#define IDS_SYSOPT_4038                         4038
#define IDS_SYSOPT_4039                         4039
#define IDS_SYSOPT_4040                         4040
#define IDS_SYSOPT_4041                         4041
#define IDS_SYSOPT_4042                         4042
#define IDS_SYSOPT_4043                         4043
#define IDS_SYSOPT_4044                         4044
#define IDS_SYSOPT_4045                         4045
#define IDS_SYSOPT_4046                         4046
#define IDS_SYSOPT_4047                         4047
#define IDS_SYSOPT_4048                         4048
#define IDS_SYSOPT_4049                         4049
#define IDS_SYSOPT_4050                         4050
#define IDS_SYSOPT_4051                         4051
#define IDS_SYSOPT_4052                         4052
#define IDS_SYSOPT_4053                         4053
#define IDS_SYSOPT_4054                         4054
#define IDS_SYSOPT_4055                         4055
#define IDS_SYSOPT_4056                         4056
#define IDS_SYSOPT_4057                         4057
#define IDS_SYSOPT_4058                         4058
#define IDS_SYSOPT_4059                         4059
#define IDS_SYSOPT_4060                         4060
#define IDS_SYSOPT_4061                         4061
#define IDS_SYSOPT_4062                         4062
#define IDS_SYSOPT_4063                         4063
#define IDS_SYSOPT_4064                         4064
#define IDS_SYSOPT_4065                         4065
#define IDS_SYSOPT_4066                         4066
#define IDS_SYSOPT_4067                         4067
#define IDS_SYSOPT_4068                         4068
#define IDS_SYSOPT_4069                         4069
#define IDS_SYSOPT_4070                         4070
#define IDS_SYSOPT_4071                         4071
#define IDS_SYSOPT_4072                         4072
#define IDS_SYSOPT_4073                         4073
#define IDS_SYSOPT_4074                         4074
#define IDS_SYSOPT_4075                         4075
#define IDS_SYSOPT_4076                         4076
#define IDS_SYSOPT_4077                         4077
#define IDS_SYSOPT_4078                         4078
#define IDS_SYSOPT_4079                         4079
#define IDS_SYSOPT_4080                         4080
#define IDS_SYSOPT_4081                         4081
#define IDS_SYSOPT_4082                         4082
#define IDS_SYSOPT_4083                         4083
#define IDS_SYSOPT_4084                         4084
#define IDS_SYSOPT_4085                         4085
#define IDS_SYSOPT_4086                         4086
#define IDS_SYSOPT_4087                         4087
#define IDS_SYSOPT_4088                         4088
#define IDS_SYSOPT_4089                         4089
#define IDS_SYSOPT_4090                         4090
#define IDS_SYSOPT_4091                         4091
#define IDS_SYSOPT_4092                         4092
#define IDS_SYSOPT_4093                         4093
#define IDS_SYSOPT_4094                         4094
#define IDS_SYSOPT_4095                         4095
#define IDS_SYSOPT_4096                         4096
#define IDS_SYSOPT_4097                         4097
#define IDS_SYSOPT_4098                         4098
#define IDS_SYSOPT_4099                         4099
