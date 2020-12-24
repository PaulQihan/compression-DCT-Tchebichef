#include "../include/huffman.h"

HuffmanDecode::HuffmanDecode() {

}
HuffmanDecode::~HuffmanDecode() {

}

void HuffmanDecode::decodeString(string & total, map<string, int> & table_map, int total_bit_length, Mat &res, int rows, int cols) {
    int index = 0;
    int cur = 1;
    int head = 0;
    while (head < total_bit_length) {
        if (total[head] != '1' && total[head] != '0') {
            head++;
            cur = head + 1;
            continue;
        }
        if (table_map.count(total.substr(head, cur - head))) {
            res.at<int>(index - rows * (index/rows), index/rows) = table_map[total.substr(head, cur - head)];
            head = cur;
            cur = head + 1;
            //cout << index/cols << ", " << index-cols * (index/cols) <<  endl;
            index++;
        } else {
            cur++;
        }
    }    
    Mat tmp(Size(cols, rows), CV_32S,  Scalar(123));
    res = res - tmp;
    res.convertTo(res, CV_32F, 1.0 / 255);
}

Mat HuffmanDecode::decode(string path, int height, int width) {
    string table[256];
    string table_path = "table_";
    table_path = table_path + path;
    FILE* fp = fopen(table_path.c_str(), "rb");
    for (int i = 0; i < 256; i++) {
        char tmp[30];
        fscanf(fp, "%s", tmp);
        table[i] = table[i] + tmp;
    }
    fclose(fp);

    map<string, int> table_map;
    for (int i = 0; i < 256; i++) {
        table_map[table[i]] = i;
    }

    fp = fopen(path.c_str(), "rb");
    int buffer;
    int total_bit_length;
    fread(&total_bit_length, sizeof(int), 1, fp);

    int times = total_bit_length / 32 + 1;
    string str = "";
    int *words = new int[times];
    fread(words, sizeof(int), times, fp);
    for (int i = 0; i < times; i++) {
        bitset<32> bits(words[i]);
        string tmp = bits.to_string();
        for (int j = 0; j < 32; j++) {
            str += tmp[j];
        }
    }
    fclose(fp);
    Mat res(Size(width, height), CV_32S);
    decodeString(str, table_map, total_bit_length, res, height, width);
    return res;    
}