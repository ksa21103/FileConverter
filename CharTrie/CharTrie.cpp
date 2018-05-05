// CharTrie.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "TrieData.h"

int main()
{
    setlocale(LC_ALL, "Russian");

    TrieStrings::StringOfCharsZeroEnd<char> keys[] = { "альфа", "аЛьфа", "аМьфа", "аАьфа", "бета", "гамма", "дельта", "эпсилон" };
    TrieStrings::StringOfCharsZeroEnd<char> keys_other[] = { "дельта", "аМьфа", "аАьфа", "бета", "гамма", "эпсилон", "аЛьфа", "альфа" };

    Trie::Trie<char, int> tree1;
    std::cout << "----------------------- добавление пар ключ/значение в tree1 :" << std::endl;
    {
        int val = 1;
        for (const auto& key : keys)
        {
            std::cout << "tree1.addKeyValue(\"" << key.getStr() << "\", " << val << std::endl;
            tree1.addKeyValue(key, val++);

            std::cout << "\t\t-- состояние tree1 после добавления очередной записи:" << std::endl;
            for (auto it = tree1.cbegin(); it != tree1.cend(); ++it)
            {
                std::cout << "\t\t\t" << it.getString().getStr() << " : " << it.getValue() << std::endl;
            }
        }
    }

    std::cout << "----------------------- добавление пар ключ/значение в tree_other :" << std::endl;
    Trie::Trie<char, int> tree_other;
    {
        int val = 1;
        for (const auto& key : keys_other)
        {
            std::cout << "tree_other.addKeyValue(\"" << key.getStr() << "\", " << val << std::endl;
            tree_other.addKeyValue(key, val++);

            std::cout << "\t\t-- состояние tree_other после добавления очередной записи:" << std::endl;
            for (auto it = tree_other.cbegin(); it != tree_other.cend(); ++it)
            {
                std::cout << "\t\t\t" << it.getString().getStr() << " : " << it.getValue() << std::endl;
            }
        }
    }

    std::cout << "-----------------------" << std::endl;
    for (const auto& key : keys)
    {
        std::cout << key.getStr() << " : " << tree1.find(key) << std::endl;
    }

    std::cout << "-----------------------" << std::endl;
    for (auto it = tree1.cbegin(); it != tree1.cend(); ++it)
    {
        std::cout << it.getString().getStr() << " : " << it.getValue() << std::endl;
    }

    std::cout << "-----------------------" << std::endl;
    for (const auto& key : keys_other)
    {
        std::cout << key.getStr() << " : " << tree_other.find(key) << std::endl;
    }

    std::cout << "-----------------------" << std::endl;
    for (auto it = tree_other.cbegin(); it != tree_other.cend(); ++it)
    {
        std::cout << it.getString().getStr() << " : " << it.getValue() << std::endl;
    }

    std::cout << "-----------------------" << std::endl;
    TrieStrings::StringOfCharsZeroEnd<char> keys1[] = { "а", "аМ", "бетЯ", "бет" };
    for (const auto& key : keys1)
    {
        std::cout << "Удаление ключа \"" << key.getStr() << "\" - ";

        if (tree1.removeKey(key))
        {
            std::cout << "выполнено";
        }
        else
        {
            std::cout << "не выполнено";
        }
        std::cout << ";" << std::endl;
    }

    std::cout << "-----------------------" << std::endl;
    for (const auto& key : keys)
    {
        std::cout << key.getStr() << " : " << tree1.find(key) << std::endl;
    }

    for (const auto& key : keys1)
    {
        std::cout << key.getStr() << " : " << tree1.find(key) << std::endl;
    }

    return 0;
}

