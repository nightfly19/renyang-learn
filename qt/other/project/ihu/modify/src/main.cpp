/*
 *  IHU -- I Hear U, easy VoIP application using Speex and Qt
 *
 *  Copyright (C) 2003-2008 Matteo Trotta - <mrotta@users.sourceforge.net>
 *
 *  http://ihu.sourceforge.net
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <qapplication.h>

#include "Config.h"
#include "Error.h"
#include "Ihu.hpp"
#include "IhuNoGui.hpp"

#ifdef HAVE_CONFIG_H
// renyang - 只有當執行完./configure後才會出現config.h這一個檔案
#include "config.h"
#endif

static Ihu *ihu = NULL;
static IhuNoGui *ihuNoGui = NULL;

// renyang - 結束程式 - 當使用者使用ctrl+c，則會跳到此處執行此函式
// renyang - 因signal(SIGINT, terminate);的關係
void terminate(int val)
{
	fprintf(stderr, "SIGINT caught. Terminating...\n");
	if (ihu)
	{
		ihu->quit();
	}
	else if (ihuNoGui)
	{
		ihuNoGui->quit();
		delete ihuNoGui;
		exit(0);
	}
}

void usage(int ret)
{
	fprintf(stdout, "Welcome to IHU v. %s by Matteo Trotta <%s>\nUSAGE\n\tihu [OPTIONS]\n", PACKAGE_VERSION, PACKAGE_BUGREPORT);
	fprintf(stdout, "OPTIONS\n\t--conf\tFILE\tUse FILE as configuration file\n\t--wait\t\tWait for incoming calls\n");
	fprintf(stdout, "\t--call\tHOST\tCall HOST (DNS or IP address)\n\t--file\tFILE\tPlay a IHU FILE\n");
	fprintf(stdout, "\t--encrypt\tEncrypt the outgoing stream\n\t--noinput\tDisable audio INPUT\n\t--nooutput\tDisable audio OUTPUT\n");
	fprintf(stdout, "\t--nogui\t\tStart without GUI (no interaction)\n\n");
	// renyang - 當ret!=0, 則表示缺少參數
	if (ret)
		fprintf(stderr, "Error: one or more argument is missing.\n");
	exit(ret);
}

int main( int argc, char **argv )
{
	// renyang - 預設是否要使用GUI介面
	bool enableGui = true;
	int ret = 0;

	// renyang - 當使用者用ctrl+c要結束程式時,會被程式攔截到,接著去執行terminate這一個副程式
	signal(SIGINT, terminate);
	
	/*
		Those options are useful for quick launch (at least for me ;),
		but I don't think it's a good idea to use them (they might be buggy),
		in the future I may use standard GNU getopt, but for the moment
		please use the graphic interface instead.
	*/
	int waitcall = 0, call = 0, file = 0, muteMic = 0, muteSpk = 0, encrypt = 0, conf = 0;
	char* cfg = NULL;

	// renyang - 在開始執行檔案時, 設定相關的參數
	for(int i=1; i<argc; i++)
	{
		// renyang - 列印出menu
		if (strcmp(argv[i], "--help")==0)
			usage(0);
		// renyang - 設定不使用GUI
		else if (strcmp(argv[i], "--nogui")==0)
			enableGui = false;
		// renyang - 使用設定檔, 要用i+1是因為後面的那一個參數就是設定檔的檔案, 設定要使用的檔案為下一個參數(i+1)
		else if (strcmp(argv[i], "--conf")==0)
			conf = i+1;
		// renyang - 等待電話
		else if (strcmp(argv[i], "--wait")==0)
			waitcall = 1;
		// renyang - 打電話, 要使用i+1也是因為下一個參數正是要撥打的電話
		else if (strcmp(argv[i], "--call")==0)
			call = i+1;
		// renyang - 撥放IHU的檔案, 下一個參數就是要撥放的檔案
		else if (strcmp(argv[i], "--file")==0)
			file = i+1;
		// renyang - 設定不使用audio input
		else if (strcmp(argv[i], "--noinput")==0)
			muteMic = 1;
		// renyang - 設定不使用audio output
		else if (strcmp(argv[i], "--nooutput")==0)
			muteSpk = 1;
		// renyang - 設定後面的那一個檔案要編碼
		else if (strcmp(argv[i], "--encrypt")==0)
			encrypt = 1;
	}
	
	// renyang - 偵測錯誤
 	if (conf)
	{
		if (argc < (conf + 1))
			usage(1);
	}
	if (file)
	{
		if (argc < (file + 1))
			usage(1);
	}
 	if (call)
	{
		if (argc < (call + 1))
			usage(1);
	}

	try
	{
		// renyang - 使用設定檔
		if (conf)
		{
			cfg = argv[conf];
			fprintf(stdout, "Using '%s' as configuration file\n", argv[conf]);
		}

		// renyang - 建立一個config物件, 使用的參數就是這一個檔案
		Config ihuconfig(cfg);
		
		// renyang - 依enableGui來決定是使用NoGui還是Gui的版本
		// renyang - 若使用這一行的話, 則不在X11也是可以執行
		QApplication a( argc, argv, enableGui );
		
		if (enableGui)
		{
			// renyang - 產生一個Ihu的物件指標, 使用剛剛建立的設定值
			ihu = new Ihu(ihuconfig);
			// renyang - 設定ihu為主要的GUI畫面, 當這一個主要的GUI畫面被結束的話, 整個應用程式就結束了
			// renyang - 另外, 值得一提的就是, 在Qt4中, 不需要設定MainWidget
			a.setMainWidget( ihu );
			
			// renyang - 初始化ihu
			ihu->initIhu();
			
			// renyang - 靜音
			if (muteMic)
				ihu->disableIn();
			// renyang - 麥克風沒有收音
			if (muteSpk)
				ihu->disableOut();
			// renyang - 封包加密
			if (encrypt)
				ihu->cryptOn();
			
			// renyang - 等待別人打電話進來
			if (waitcall)
				ihu->waitForCalls();
	
			// renyang - 撥放IHU檔案
			if (file)
				ihu->playFile(argv[file]);
			// renyang - 撥打給某一個ip檔案
			else if (call)
				ihu->call(argv[call]);
	
			// renyang - 執行GUI的程式
			ret = a.exec();
	
			// renyang - 程式準備結束, 刪除所建立的ihu空間, 它會把所有的子類別也同時刪除
			delete ihu;
		}
		else
		{
			// renyang - 這一個區塊是表是沒有使用到GUI的部分

			fprintf(stdout, "Starting IHU without GUI...\nPress CTRL-C to terminate\n");

			// renyang - 所包含的參數必需要包含waitcall, call, file這兩個其中一個
			if (!waitcall && !call && !file)
				usage(1);
			
			// renyang - 建立一個沒有GUI的指標物件, 且同時初始化IhuNoGui的configure
			ihuNoGui = new IhuNoGui(ihuconfig);
			
			// renyang - 初始化no-GUI的物件
			ihuNoGui->initIhu();
			
			// renyang - 聲音沒有辦法輸入
			if (muteMic)
				ihuNoGui->disableIn();
			// renyang - 聲音沒有辦法輸出
			if (muteSpk)
				ihuNoGui->disableOut();
			// renyang - 設定是否加密
			if (encrypt)
				ihuNoGui->cryptOn();
			
			if (waitcall)
				ihuNoGui->waitForCalls();
			else if (call)
				ihuNoGui->call(argv[call]);
			else if (file)
				ihuNoGui->playFile(argv[file]);
			
			ret = a.exec();
	
			delete ihuNoGui;
		}

		// renyang - 把設定寫入Config中
		ihuconfig.writeConfig();
	}
	// renyang - 抓取錯誤
	// renyang - 由子程式出現的錯誤，若在同一個層級沒有人來處理它, 則會一直往呼叫它的層級去尋找
	// renyang - 是否有可以處理些例外的catch
	catch (Error e)
	{
		fprintf(stderr, "ABORTED.\nError: %s\n", e.getError());
		ret = 1;
	}
	
	return ret;
}
