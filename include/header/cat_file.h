#ifndef __CAT_FILE_H__
#define __CAT_FILE_H__

#include <header/hash_object.h>
#include <openssl/sha.h>
#include <zlib.h>

// @brief ��ȡѹ���Ķ��󲢽�ѹ��
// @param input ��������
// @param input_len �������ݳ���
// @param output �������
// @param output_len ������ݳ���
// @return 0:�ɹ���-1:ʧ��
int decompress_data(const char* input, size_t input_len, char** output, size_t* output_len);

// ��дһ����cat_file�������Ƶ�read_compressed_object��ֻ����ֻ��Ҫ����·���ͽ�ѹ����ļ����ļ�����
// @param file �ļ�·��
// @param decompressed_data ��ѹ����ļ�
// @param decompressed_len ��ѹ����ļ�����	
// @return 0:�ɹ���-1:ʧ��
int read_compressed_object(const char* file, char** decompressed_data, size_t* decompressed_len);

// ��ȡ�ļ�������������
void cat_file(const char* hash);


#endif