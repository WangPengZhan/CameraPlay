#include "MoudleConfig.h"

#include <time.h>

#if defined(WIN32)
#include <Windows.h>
#include <direct.h>
#include <io.h> //C (Windows)    access
#else
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

void Sleep(long mSeconds)
{
    usleep(mSeconds * 1000);
}

#endif

int MoudleConfig::VERSION = 1;
char MoudleConfig::VERSION_NAME[32] = "1.0.0";

MoudleConfig::MoudleConfig()
{

}

void MoudleConfig::mkdir(char *dirName)
{
#if defined(WIN32)
    ///���Ŀ¼������ �򴴽���
    if (access(dirName, 0)!=0)
    {
        _mkdir(dirName);
    }
#else
    ///���Ŀ¼������ �򴴽���
    if (access(dirName, R_OK)!=0)
    {
        char cmd[128];
        sprintf(cmd,"mkdir %s", dirName);
        system(cmd);
    }
#endif
}

void MoudleConfig::mkpath(char *path)
{
#if defined(WIN32)
        ///windows�����ļ������� ·�����Ƿ�б�� ���������Ҫ�滻һ��
        char dirPath[128] = {0};
        strcpy(dirPath, path);

        MoudleConfig::replaceChar(dirPath, '/', '\\');

        ///���Ŀ¼������ �򴴽���
        if (access(dirPath, 0)!=0)
        {
    //        _mkdir(dirPath);
            char cmd[128];
            sprintf(cmd,"mkdir %s", dirPath);
            system(cmd);
        }

#else
    ///���Ŀ¼������ �򴴽���
    if (access(path,R_OK)!=0)
    {
        char cmd[128];
        sprintf(cmd,"mkdir %s -p",path);
        system(cmd);
    }
#endif
}

void MoudleConfig::removeDir(char *dirName)
{
    if (strlen(dirName) <= 0) return;

    if (access(dirName, 0) != 0 ) //�ļ��в�����
    {
        return;
    }

#if defined(WIN32)

    ///ɾ�������ļ�
    char cmd[128];
    sprintf(cmd,"rd /s /q \"%s\"", dirName);
    system(cmd);

#else

    char cmd[128];
    sprintf(cmd,"rm -rf \"%s\"",dirName);
    system(cmd);

#endif
}

void MoudleConfig::removeFile(const char *filePath)
{
    if (filePath == NULL || strlen(filePath) <= 0) return;

#if defined(WIN32)

        ///ɾ�������ļ�
        remove(filePath);

#else
        ///ɾ�������ļ�
        char cmd[128] = {0};
        sprintf(cmd,"rm -rf \"%s\"",filePath);
        system(cmd);
#endif
}

void MoudleConfig::copyFile(const char *srcFile, const char *destFile)
{

#if defined(WIN32)
        CopyFileA(srcFile, destFile, FALSE);
#else

        ///���ļ�������Զ�˷�����
        char copyfilecmd[512];
        sprintf(copyfilecmd,"cp \"%s\" \"%s\"", srcFile, destFile);
        system(copyfilecmd);

#endif
}

void MoudleConfig::replaceChar(char *string, char oldChar, char newChar)
{
    int len = strlen(string);
    int i;
    for (i = 0; i < len; i++){
        if (string[i] == oldChar){
            string[i] = newChar;
        }
    }
}

std::string MoudleConfig::removeFirstAndLastSpace(std::string &s)
{
    if (s.empty())
    {
        return s;
    }
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

std::string MoudleConfig::getIpFromRtspUrl(std::string rtspUrl)
{
    std::string strIP;
    std::string strUrl = rtspUrl;
    if(strUrl.find("@") == std::string::npos)
    {
        long nPos1 = strUrl.find("//");
        long nPos2 = strUrl.rfind(":");
        if(nPos1 != std::string::npos && nPos2 != std::string::npos)
        {
            long nOffset = nPos2 - nPos1 - strlen("//");
            strIP = strUrl.substr(nPos1 + strlen("//"), nOffset);
        }
    }
    else
    {
        long nPos1 = strUrl.find("@");
        long nPos2 = strUrl.rfind(":");
        if(nPos1 != std::string::npos && nPos2 != std::string::npos)
        {
            long nOffset = nPos2 - nPos1 - strlen("@");
            strIP = strUrl.substr(nPos1 + strlen("@"), nOffset);

            int index = strIP.find("/");
            strIP = strIP.substr(0, index);
        }
    }
    return strIP;
}

void MoudleConfig::mSleep(int mSecond)
{
#if defined(WIN32)
    Sleep(mSecond);
#else
    usleep(mSecond * 1000);
#endif
}

int64_t MoudleConfig::getTimeStamp_MilliSecond()
{

    int mSecond = 0; //��ǰ������

#if defined(WIN32)

    SYSTEMTIME sys;
    GetLocalTime( &sys );

    mSecond = sys.wMilliseconds;

#else

    struct timeval    tv;
    struct timezone tz;

    struct tm         *p;

    gettimeofday(&tv, &tz);
    p = localtime(&tv.tv_sec);

    mSecond = tv.tv_usec / 1000;


#endif

    int64_t timeStamp = time(NULL) * 1000 + mSecond;

    return timeStamp;

}
