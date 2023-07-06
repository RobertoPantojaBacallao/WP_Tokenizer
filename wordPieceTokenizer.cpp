#include "wordPieceTokenizer.h"
#include <iostream>

wordPieceTokenizer::wordPieceTokenizer(std::vector<std::string> corpus) {
	this->corpus = corpus;
}

std::vector<std::string> wordPieceTokenizer::splitCorpus(std::vector<std::string> corpus, const char* delim) {
    std::vector<std::string> tokens;
    std::string word = "";

    for (auto& str : corpus) {
        for (auto& a : str) {
            if (a != *delim && !ispunct(a) && a != *"\n")
                word += a;
            else {
                if (strcmp(word.c_str(), "") != 0 && find(tokens.begin(), tokens.end(), word) == tokens.end()) {
                    tokens.push_back(word);
                }
                word = "";
            }
        }
    }

    if (word != "")
        tokens.push_back(word);

    return tokens;
}

std::vector<std::string> wordPieceTokenizer::splitText(std::string str, const char* delim) {
    std::vector<std::string> tokens;
    std::string word = "";

    for (auto& a : str) {
        if (a != *delim && !ispunct(a) && a != *"\n")
            word += a;
        else {
            if (strcmp(word.c_str(), "") != 0 && find(tokens.begin(), tokens.end(), word) == tokens.end()) {
                tokens.push_back(word);
            }
            word = "";
        }
    }

    if (word != "")
        tokens.push_back(word);

    return tokens;
}

std::map<std::string, int> wordPieceTokenizer::getTokenFrequencies(std::vector<std::string> texts, const char* delim) {
    std::map<std::string, int> tokenFrequency;
    std::string word = "";

    for (auto& str : texts) {
        for (auto& a : str) {
            if (a != *delim && !ispunct(a) && a != *"\n")
                word += a;
            else {
                if (strcmp(word.c_str(), "") != 0)
                    tokenFrequency[word]++;
                word = "";
            }
        }
    }

    return tokenFrequency;
}

std::map<std::string, std::vector<std::string>> wordPieceTokenizer::splitWords(std::map<std::string, int> tokenFrequency) {
    std::map<std::string, std::vector<std::string> > splits;
    std::string chr;

    for (auto& a : tokenFrequency) {
        splits.insert({ a.first, std::vector<std::string>{} });
        chr = a.first.at(0);
        splits[a.first].push_back(chr);

        for (int i = 1; i < a.first.size(); i++) {
            chr = a.first.at(i);
            splits[a.first].push_back("##" + chr);
        }
    }

    return splits;
}

std::string wordPieceTokenizer::mergePair(const std::string p1, const std::string p2, std::map<std::string, std::vector<std::string>>& splits) {
    std::vector<std::string> split;
    std::string merge;
    int i;

    for (auto& a : splits) {
        split = splits[a.first];

        if (split.size() == 1)
            continue;
        i = 0;
        while (i < split.size() - 1) {
            if (strcmp(split[i].c_str(), p1.c_str()) == 0 && strcmp(split[i + 1].c_str(), p2.c_str()) == 0) {
                if (p2.at(0) == *"#")
                    merge = p1 + p2.substr(2, p2.size() - 2);
                else
                    merge = p1 + p2;

                split[i] = merge;
                split.erase(next(split.begin() + i));
            }
            else
                i++;
        }

        splits[a.first] = split;

    }

    return merge;
}

std::map<std::pair<std::string, std::string>, double> wordPieceTokenizer::getPairScores(std::map<std::string, std::vector<std::string>> splits, std::map<std::string, int> tokenFrequencies) {
    std::map<std::string, int> letterFrequencies{};
    std::map<std::pair<std::string, std::string>, double> pairFrequencies{};
    std::vector<std::string> split;
    std::pair<std::string, std::string> pair;
    
    for (auto& freq : tokenFrequencies) {
        split = splits[freq.first];
        if (split.size() == 1) {
            letterFrequencies[split[0]] += freq.second;
            continue;
        }
    
        for (int i = 0; i < split.size() - 1; i++) {
            pair = { split[i], split[i + 1] };
            letterFrequencies[split[i]] += freq.second;
            pairFrequencies[pair] += freq.second;
        }
        letterFrequencies[split[split.size() - 1]] += freq.second;
    }
    
    for (auto& freq : pairFrequencies)
        freq.second = freq.second / (letterFrequencies[freq.first.first] * letterFrequencies[freq.first.second]);
    
    return pairFrequencies;
}

std::vector<std::string> wordPieceTokenizer::tokenizeWord(std::string text, std::vector<std::string> vocab) {
    std::vector<std::string> tokens;
    std::string sub;
    int i;

    while (text.size() > 0) {
        i = text.size();
        sub = text.substr(0, i);
        while (i > 0 && find(vocab.begin(), vocab.end(), sub) == vocab.end()) {
            i--;
            sub = text.substr(0, i);
        }

        if (i == 0 && find(vocab.begin(), vocab.end(), sub) == vocab.end())
            return std::vector<std::string>{"[UNK]"};
        tokens.push_back(sub);
        text = text.substr(i, text.size() - 1);
        if (text.size() > 0) {
            text = "##" + text;
        }
    }

    return tokens;
}

std::vector<std::string> wordPieceTokenizer::getVocab(std::map<std::string, std::vector<std::string>>& splits, int vocabSize, std::map<std::string, int> tokenFrequencies) {
    std::vector<std::string> vocab({ "[PAD]", "[UNK]", "[CLS]", "[SEP]", "[MASK]" });

    for (auto& a : splits) {
        for (auto& b : a.second) {
            vocab.push_back(b);
        }
    }

    std::pair<std::string, std::string> bestPair;
    int size = vocab.size();
    double maxScore;

    while (vocab.size() - size < vocabSize) {
        auto scores = getPairScores(splits, tokenFrequencies);
        bestPair = {};
        maxScore = 0.0;
        for (auto& a : scores) {
            if (maxScore == NULL || maxScore < a.second) {
                bestPair = a.first;
                maxScore = a.second;
            }
        }
        vocab.push_back(mergePair(bestPair.first, bestPair.second, splits));
    }

    sort(vocab.begin() + 5, vocab.end());

    return vocab;
}

std::vector<std::vector<std::string>> wordPieceTokenizer::tokenize(std::vector<std::string> texts, std::vector<std::string> vocab) {
    if(vocab.empty())
        vocab = this->vocab;

    std::vector<std::vector<std::string>> result;
    std::vector<std::string> temp;

    for (auto& text : texts) {
        temp = {};
        for (auto& a : splitText(text, " ")) {
            for (auto& b : tokenizeWord(a, vocab)) {
                temp.push_back(b);
            }
        }
        result.push_back(temp);
    }

    return result;
}

void wordPieceTokenizer::train(int vocabSize) {
    std::map<std::string, int> tokenFrequencies = getTokenFrequencies(this->corpus);
    std::map<std::string, std::vector<std::string>> splits = splitWords(tokenFrequencies);
    this->vocab = getVocab(splits, vocabSize, tokenFrequencies);
}