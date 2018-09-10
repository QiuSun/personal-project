#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <String>
#include <fstream>
#include <assert.h>
#include "share.h"
#include "CountWord.h"
#include "CountChar.h"
#include "CountLine.h"
#include "ShowResult.h"
#include "GetFirstTenWords.h"

extern bool g_has_got_map;
extern bool g_has_got_lines;
extern bool g_has_got_characters;

extern int g_characters;
extern int g_lines;
extern map<string, int> g_word_count_map;

// 字符是否是字母
bool CharAlphaJudge(char ch)
{
	if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
		return true;
	return false;
}

// 字符是否是字母 or 数字
bool CharAlphaNumberJudge(char ch)
{
	if (('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
		return true;
	return false;
}

// 检查word的合法性
bool WordCheck(string word)
{
	if (word.size() < 4)
		return false;

	for (int i = 0; i < 4; i++)
		if (!CharAlphaJudge(word[i])) return false;
	return true;
}

// 检查一行是否有可显示的字符
bool HaveVisibleChar(string line)
{
	int length = line.size();
	for (int i = 0; i < length; i++)
	{
		if (line[i] != ' ') return true;
	}
	return false;
}

// 得到词频字典和字符数
void GetWordCountMap(string file_location)
{
	ifstream instream;
	instream.open(file_location, ios::binary);
	assert(instream.is_open());

	string line;

	// 全局变量初始化
	g_lines = 0;
	g_characters = 0;
	g_word_count_map.clear();

	while (true)
	{
		bool have_next_line = GetLine(instream, line);

		cout << line << '*' << endl;

		int word_left_pos = 0;
		int word_right_pos = 0;
		int word_length = 0;
		int line_length = line.size();

		while (word_left_pos < line_length)
		{
			while (!CharAlphaNumberJudge(line[word_left_pos]) && word_left_pos < line_length)
				word_left_pos++;
			word_right_pos = word_left_pos;
			while (((word_right_pos + 1) < line_length) && (CharAlphaNumberJudge(line[word_right_pos + 1])))
				word_right_pos++;
			word_length = word_right_pos - word_left_pos + 1;
			if (word_length >= 4)
			{
				string word = line.substr(word_left_pos, word_length);
				// word转小写
				transform(word.begin(), word.end(), word.begin(), ::tolower);
				if (WordCheck(word))
				{
					if (g_word_count_map.count(word) == 0)
						g_word_count_map[word] = 1;
					else g_word_count_map[word] += 1;
				}
			}
			word_left_pos = word_right_pos + 1;
		}
		if (!have_next_line) break;
	}

	// 修改标记位
	g_has_got_map = true;
	g_has_got_lines = true;
	g_has_got_characters = true;

	// 关闭文件
	instream.close();
}

// return a orderd vector of WordNode
vector<WordNode> GetFirstTenWords(string file_location)
{
	if (g_has_got_map == false)
		GetWordCountMap(file_location);

	vector<WordNode> word_node_vectors;

	map<string, int>::iterator it = g_word_count_map.begin();

	for (int i = 0; it != g_word_count_map.end(); i++, it++)
		word_node_vectors.push_back(WordNode(it->first, it->second));

	// according to the overload function, sort the g_word_node_vectors
	sort(word_node_vectors.begin(), word_node_vectors.end());

	return word_node_vectors;
}

int CountLine(string file_location)
{
	// 如果已经得到 直接返回
	if (g_has_got_lines)
		return g_lines;

	ifstream instream;
	instream.open(file_location, ios::in);
	assert(instream.is_open());

	string line;
	int valid_lines = 0;
	while (getline(instream, line))
	{
		if (HaveVisibleChar(line)) valid_lines++;
	}

	instream.close();

	return valid_lines;
}

int CountChar(string file_location)
{
	// 如果已经得到 就直接返回
	if (g_has_got_characters)
		return g_characters;

	ifstream instream;

	// 通过二进制形式载入文件
	instream.open(file_location, ios::binary);
	assert(instream.is_open());

	char ch;
	int count_char = 0;
	while (instream.get(ch)) count_char += 1;

	instream.close();
	return count_char;
}

int CountWord(string file_location)
{
	// 如果没有获得字典 就获得一次字典
	if (!g_has_got_map)
		GetWordCountMap(file_location);

	int count_word = g_word_count_map.size();
	return count_word;
}

void ShowResult(int characters, int words, int lines, vector<WordNode> first_ten_words)
{
	ofstream outstream;
	outstream.open("result.txt");
	outstream << "characters: " << characters << endl;
	outstream << "words: " << words << endl;
	outstream << "lines: " << lines << endl;

	int num_should_show = min(words, 10);
	for (int i = 0; i < num_should_show; i++)
		outstream << first_ten_words[i].word << ": " << first_ten_words[i].count << endl;

	outstream.close();
}

// 自定义getline函数 返回后续还有没有字符
bool GetLine(ifstream &instream, string &line)
{
	char ch;
	bool have_visible_char = false;
	line.clear();
	while (instream.get(ch))
	{
		g_characters++;
		if (ch == '\n')
		{
			if (have_visible_char) g_lines++;
			return true;
		}
		else if (ch == '\r')
		{
			continue;
		}
		else
		{
			if (ch != ' ') have_visible_char = true;
			line += ch;
		}
	}
	if (have_visible_char) g_lines++;
	return false;
}