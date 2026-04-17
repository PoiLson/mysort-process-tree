#pragma once


void getFlags(int argc, char* argv[], int* k, const char** filename, const char** sorting1, const char** sorting2);
int openPipeForWrite(const char* fifo_path);

