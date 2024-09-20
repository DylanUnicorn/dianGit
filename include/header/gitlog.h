#ifndef _LOG_H_
#define _LOG_H_

// 更新.git/logs文件，追加提交记录
void update_commit_log(const char* commit_hash, const char* commit_msg, const char* timestamp);

// 显示提交日志
void display_log();

#endif