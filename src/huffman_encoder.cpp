#include "../include/huffman.h"


HuffmanEncode::HuffmanEncode() {

}

HuffmanEncode::~HuffmanEncode() {

}

int HuffmanEncode::findMinNode(Node* nodes, int length) {
    int index = -1;
    for (int i = 0; i < length; i++) {
        if ((index == -1 || nodes[i].count < nodes[index].count) && !nodes[i].removed && nodes[i].count > 0) {
            index = i;
        }
    }
    if (index != -1) {
        nodes[index].removed = 1;
    }
    return index;
}

int HuffmanEncode::buildTree(Node* nodes, int* counts) {
    for (int i = 0; i < 256; i++) {
        nodes[i].left = -1;
        nodes[i].right = -1;
        nodes[i].count = counts[i];
        nodes[i].removed = 0;
    }
    int length = 256;

    while (1) {
        int l = findMinNode(nodes, length);
        if (l == -1) {
            break;
        }
        int r = findMinNode(nodes, length);
        if (r == -1) {
            break;
        }
        nodes[length].left = l;
        nodes[length].right = r;
        nodes[length].count = nodes[l].count + nodes[r].count;
        nodes[length].removed = 0;
        length++;
    }
    return length;
}

void HuffmanEncode::buildTable(Node* nodes, int pos, string bits, string * table) {
    int l = nodes[pos].left;
    int r = nodes[pos].right;
    if (nodes[pos].left == -1 && nodes[pos].right == -1) {
        table[pos] = bits;
        return;
    }
    buildTable(nodes, r, bits + "1", table);
    buildTable(nodes, l, bits + "0", table);
}

int HuffmanEncode::encode(Mat data, string path) {

    data.convertTo(data, CV_32S, 255);
    int rows = data.rows;
    int cols = data.cols;
    Mat tmp(Size(cols, rows), CV_32S,  Scalar(123));
    data = data + tmp;
    FILE* fp;
    int counts[256];
    memset(counts, 0, sizeof(int) * 256);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            counts[(int)data.at<int>(i, j)]++;
        }
    }

    Node nodes[256 * 2];
    int length = buildTree(nodes, counts);    
    
    string table[256];
    buildTable(nodes, length - 1, "", table);

    string table_path = "table_";
    table_path = table_path + path;
    fp = fopen(table_path.c_str(), "wb");
    for (int i = 0; i < 256; i++) {
        if (table[i].size() == 0) {
            fprintf(fp, "2\n");
        } else  {
            fprintf(fp, "%s\n", table[i].c_str());
        }
    }
    fclose(fp);   

    int total_bit_length = 0;
    for (int i = 0; i < 256; i++) {
        total_bit_length += counts[i] * table[i].size();
    }
    string total;
    int cur = 0;
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            for (int k = 0; k < table[data.at<int>(j, i)].size(); k++) {
                total += table[data.at<int>(j, i)][k];
                cur++;
            }
        }
    }
    fp = fopen(path.c_str(), "wb");
    int times = total_bit_length / 32 + 1;
    for (int i = 0; i < 32 * times - total_bit_length; i++) {
        total = total + "0";
    }
    fwrite(&total_bit_length, sizeof(int), 1, fp);
    for (int i = 0; i < times; i++) {
        bitset<32> byte(total.substr(32 * i, 32));

        unsigned long tmp = byte.to_ulong();
        fwrite(&tmp, sizeof(int), 1, fp);
    }
    fclose(fp);

    return total_bit_length;        
}

