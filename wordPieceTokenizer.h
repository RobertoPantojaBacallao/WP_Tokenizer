#pragma once
#include <vector>
#include <cstring>
#include <algorithm>
#include <map>
#include <string>

class wordPieceTokenizer {
private:
	std::vector<std::string> corpus;
	std::vector<std::string> vocab;

public:
	wordPieceTokenizer(std::vector<std::string> corpus);
	std::vector<std::string> splitCorpus(std::vector<std::string> corpus, const char* delim = " ");
	std::vector<std::string> splitText(std::string str, const char* delim = " ");
	std::map<std::string, int> getTokenFrequencies(std::vector<std::string> texts, const char* delim = " ");
	std::map<std::string, std::vector<std::string>> splitWords(std::map<std::string, int> tokenFrequency);
	std::vector<std::string> getVocab(std::map<std::string, std::vector<std::string>>& splits, int vocabSize, std::map<std::string, int> tokenFrequencies);
	std::map<std::pair<std::string, std::string>, double> getPairScores(std::map<std::string, std::vector<std::string>> splits, std::map<std::string, int> tokenFrequencies);
	std::string mergePair(const std::string p1, const std::string p2, std::map<std::string, std::vector<std::string>>& splits);
	std::vector<std::string> tokenizeWord(std::string text, std::vector<std::string> vocab);
	std::vector<std::vector<std::string>> tokenize(std::vector<std::string> texts, std::vector<std::string> vocab = {});
	void train(int vocabSize = 100);
};
