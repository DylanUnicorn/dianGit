#ifndef _LOG_H_
#define _LOG_H_

// ����.git/logs�ļ���׷���ύ��¼
void update_commit_log(const char* commit_hash, const char* commit_msg, const char* timestamp);

// ��ʾ�ύ��־
void display_log();

#endif