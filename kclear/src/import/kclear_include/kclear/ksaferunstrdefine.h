// Copyright (c) 2010 Kingsoft Corporation. All rights reserved.
// Copyright (c) 2010 The KSafe Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once


#define KSAFE_RUN_XML_ROOT			_T("xml")
#define KSAFE_RUN_XML_ITEM			_T("item")
#define KSAFE_RUN_XML_ITEM_ID		_T("id")
#define KSAFE_RUN_XML_ITEM_TYPE		_T("type")
#define KSAFE_RUN_XML_ITEM_NAME		_T("name")
#define KSAFE_RUN_XML_ITEM_DESC		_T("desc")
#define KSAFE_RUN_XML_ITEM_WARNING	_T("warning")
#define KSAFE_RUN_XML_ITEM_DISPLAY	_T("display")
#define KSAFE_RUN_XML_ITEM_APPNAME	_T("appname")
#define KSAFE_RUN_XML_ITEM_ADVICE	_T("advice")
#define KSAFE_RUN_XML_ITEM_SYSTEM	_T("system")
#define KSAFE_RUN_XML_ITEM_NODELAY	_T("nodelay")
#define KSAFE_RUN_XML_ITEM_REGPATH	_T("regpath")


class KSRunInfo
{
public:
	KSRunInfo()
	{
		nItemId = 0;
		nType	= 0;
		nAdvice	= 0;
		bNoDealyShow = FALSE;
		bSystem	= FALSE;
		bRegPath = FALSE;
	}

	DWORD		nItemId;		//每项惟一ID
	DWORD		nType;			//类型
	CString		strName;		//名字
	CString		strDesc;		//描述
	CString		strAppName;		//对应的APP的名字
	CString		strDisplay;		//界面中显示的名字
	DWORD		nAdvice;		//建议操作
	CString		strWarning;		//操作警告
	BOOL		bNoDealyShow;	//不显示延时启动
	BOOL		bSystem;		//是否是系统项
	BOOL		bRegPath;		//半匹配AppName
};