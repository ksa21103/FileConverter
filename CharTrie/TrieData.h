#pragma once

#include <vector>
#include <iterator>
#include <cctype>

#include "TrieStrings.h"

namespace Trie
{
    // Получение неопределенного значения для типа данных, хранящегося в дереве
    template <typename TValueType>
    TValueType get_undefined_value()
    {
        static_assert("Не определено значение по умолчанию");
    }

    template <>
    int get_undefined_value()
    {
        return -1;
    }

    struct compare_no_case
    {
        bool operator()(char ch1, char ch2) const
        {
            return tolower(ch1) < tolower(ch2);
        }
    };

    ////////////////////////////////////////////////////////////////////////////
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    class Node
    {
    public:

        using tree_node_type = Node<TCharType, TValueType, KeyCharLess>;

        static tree_node_type* create();
        static tree_node_type* create(TCharType keyChar);
        static tree_node_type* create(TCharType keyChar, TValueType value);
        static tree_node_type* clone(tree_node_type* node);

        Node();
        Node(TCharType keyChar);
        Node(TCharType keyChar, TValueType value);
        virtual ~Node();

        ///////////////////////////////////////////
        // Работа с ключем

        // Установить/получить символ ключа
        void                setKeyChar(TCharType keyChar);
        TCharType           getKeyChar() const;

        ///////////////////////////////////////////
        // Работа со значением

        // Определить, хранится ли значение
        bool                haveValue() const;

        // Установить/получить значение
        void                setValue(TValueType value);
        const TValueType&   getValue() const;

        ///////////////////////////////////////////
        // Работа со следющими элементами

        // Установить/получить указатель на следующий элемент дерева
        void                setNext(tree_node_type* next);
        tree_node_type*     getNext() const;

        // Получить указатель на следующий элемент для указанного символа
        /**
         * Если текущий элемент обладает указанным символом, будет возвращен он
         * Если подходящий элемент не найден, но дано разрешение на создание, он будет создан
         */
        tree_node_type*     getBrother(TCharType keyChar, bool bCreateIfNotExist, bool& bCreated);

        // Изьять указатель на следующий элемент для указанного символа
        /**
         * Выполянется изъятие элемента для указанного символа из цепочки элементов нашего уровня
         * ВНИМАНИЕ: ответстенность за удаление элемента лежит на вызывающей стороне
         */
        tree_node_type*     removeBrother(TCharType keyChar);

        ///////////////////////////////////////////
        // Работа с дочерним

        // Установить/получить указатель на дочерний элемент дерева
        void                setChild(tree_node_type* child);
        tree_node_type*     getChild() const;
        tree_node_type*     getChild(bool bCreateIfNotExist, bool& bCreated);

    private:

        bool                isLess(TCharType ch1, TCharType ch2);
        bool                isEqual(TCharType ch1, TCharType ch2);

    private:

        TCharType          m_keyChar = 0;
        TValueType         m_value   = get_undefined_value<TValueType>();
        tree_node_type*    m_next    = nullptr;
        tree_node_type*    m_child   = nullptr;
    };

    ////////////////////////////////////////////////////////////////////////////
    //
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    class const_iterator : public std::iterator<std::forward_iterator_tag, TValueType>
    {
    public:

        using string_type = TrieStrings::StringOfCharsZeroEnd<TCharType>;
        using node_type   = Node<TCharType, TValueType, KeyCharLess>;

        const_iterator();
        const_iterator(const std::vector<node_type*> nodes);
        const_iterator(const const_iterator<TCharType, TValueType, KeyCharLess>& other);
        const_iterator(const_iterator<TCharType, TValueType, KeyCharLess>&& other);

        const node_type* operator->() const;
        const node_type* operator*() const;

        const string_type&  getString() const;
        const TValueType&   getValue() const;

        const_iterator<TCharType, TValueType, KeyCharLess>& operator=(const const_iterator<TCharType, TValueType, KeyCharLess>& other);
        const_iterator<TCharType, TValueType, KeyCharLess>& operator=(const_iterator<TCharType, TValueType, KeyCharLess>&& other);
        const_iterator<TCharType, TValueType, KeyCharLess>& operator++();
        bool                                            operator==(const const_iterator<TCharType, TValueType, KeyCharLess>& other) const;
        bool                                            operator!=(const const_iterator<TCharType, TValueType, KeyCharLess>& other) const;

	protected:

		struct TrieLevelInfo
		{
            node_type* node     = nullptr;
            bool       bToChild = true;
		};
        using TrieLevelInfoVector = std::vector<TrieLevelInfo>;

        TrieLevelInfoVector m_path;
        mutable string_type m_string;
    };

    ////////////////////////////////////////////////////////////////////////////
    //
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    class iterator : public const_iterator<TCharType, TValueType, KeyCharLess>
    {
    public:

        using node_type = Node<TCharType, TValueType, KeyCharLess>;

        iterator();
        iterator(const std::vector<node_type*>& nodes);
        iterator(const iterator& other);
        iterator(iterator&& other);

        node_type* operator->() const;
        node_type* operator*() const;

        iterator<TCharType, TValueType, KeyCharLess>& operator=(const iterator& other);
        iterator<TCharType, TValueType, KeyCharLess>& operator=(iterator&& other);

        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
    };

    ////////////////////////////////////////////////////////////////////////////
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    class const_iterator;
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    class iterator;

    ////////////////////////////////////////////////////////////////////////////
    template<typename TCharType, typename TValueType, typename KeyCharLess = compare_no_case>
    class Trie
    {
    public:

        using string_type         = TrieStrings::StringOfChars<typename TCharType>;
        using iterator_type       = iterator<typename TCharType, typename TValueType, typename KeyCharLess>;
        using const_iterator_type = const_iterator<typename TCharType, typename TValueType, typename KeyCharLess>;

        Trie();
        ~Trie();

        // Добавление пары ключ/значение
        /**
         * Если ключ найден, то заменяем значение на новое.
         * Если ключ не найден, то добавляется новая пара ключ/значение
         */
        Node<typename TCharType, typename TValueType, typename KeyCharLess>* addKeyValue(const string_type& key, typename TValueType value);

        // Удаление заданного ключа (слова) из дерева
        /**
         * Удаляется и ключ, и значение
         */
        bool removeKey(const TrieStrings::StringOfChars<TCharType>& key);

        // Поиск заданного ключа (слова) в дереве
        /**
         * В случае, если слово найдено, возвращается значение.
         * Если слово не найдено, возвращается -1 (можно сделать любым другим способом)
         */
        TValueType find(const TrieStrings::StringOfChars<TCharType>& key);

        // Также, необходимо предоставить пример перебора всех значений для всех
        // слов больше заданного.
        // Пример:
        //      дерево содержит слова: {альфа, бета, гамма, дельта, эпсилон}.
        //      C каждым словом связано число.
        //      Нужно получить все числа для слов больше бета - т.е. найти числа,
        //      связанные со словами гамма, дельта, эпсилон. 
        const_iterator_type lower_bound(const TrieStrings::StringOfChars<TCharType>& key) const;

        const_iterator_type cbegin() const;
        const_iterator_type cend()   const;

    private:

        iterator_type begin() const;
        iterator_type end()   const;

        using node_type = Node<TCharType, TValueType, KeyCharLess>;

        // Создание и получение корневого узла
        node_type*                          intGetRoot();

        // Удаление корневого узла
        void                                intResetRoot();

        // Получить узел для указанного значения ключа
        iterator_type                       intGetNode(const TrieStrings::StringOfChars<TCharType>& key, size_t keyLength, bool bCreateIfNotExist);
        void                                intGetNode(const TrieStrings::StringOfChars<TCharType>& key, size_t keyLength,
                                                       std::vector<node_type*>& path,
                                                       bool bCreateIfNotExist);

    private:

        node_type* m_rootNode = nullptr;
    };
}

namespace Trie
{
    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename Node<TCharType, TValueType, KeyCharLess>::tree_node_type*
    Node<TCharType, TValueType, KeyCharLess>::create()
    {
        return new Node<TCharType, TValueType, KeyCharLess>();
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename Node<TCharType, TValueType, KeyCharLess>::tree_node_type*
    Node<TCharType, TValueType, KeyCharLess>::create(TCharType keyChar)
    {
        return new Node<TCharType, TValueType, KeyCharLess>(keyChar);
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename Node<TCharType, TValueType, KeyCharLess>::tree_node_type*
    Node<TCharType, TValueType, KeyCharLess>::create(TCharType keyChar, TValueType value)
    {
        return new Node<TCharType, TValueType, KeyCharLess>(keyChar, value);
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename Node<TCharType, TValueType, KeyCharLess>::tree_node_type*
    Node<TCharType, TValueType, KeyCharLess>::clone(typename Node<TCharType, TValueType, KeyCharLess>::tree_node_type* node)
    {
        tree_node_type* newNode = create(node->getKeyChar(), node->getValue());
        newNode->setNext(node->getNext());
        newNode->setChild(node->getChild());

        return newNode;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    Node<TCharType, TValueType, KeyCharLess>::Node()
    {
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    Node<TCharType, TValueType, KeyCharLess>::Node(TCharType keyChar)
        : m_keyChar(keyChar)
    {
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    Node<TCharType, TValueType, KeyCharLess>::Node(TCharType keyChar, TValueType value)
        : m_keyChar(keyChar),
          m_value(value)
    {
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    Node<TCharType, TValueType, KeyCharLess>::~Node()
    {
        delete m_child;
        m_child = nullptr;

        delete m_next;
        m_next = nullptr;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    void
    Node<TCharType, TValueType, KeyCharLess>::setKeyChar(TCharType keyChar)
    {
        m_keyChar = keyChar;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    inline
    TCharType
    Node<TCharType, TValueType, KeyCharLess>::getKeyChar() const
    {
        return m_keyChar;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    bool
    Node<TCharType, TValueType, KeyCharLess>::haveValue() const
    {
        return m_value == get_undefined_value<TValueType>();
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    void
    Node<TCharType, TValueType, KeyCharLess>::setValue(TValueType value)
    {
        m_value = value;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    inline
    const TValueType&
    Node<TCharType, TValueType, KeyCharLess>::getValue() const
    {
        return m_value;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    void
    Node<TCharType, TValueType, KeyCharLess>::setNext(typename Node<TCharType, TValueType, KeyCharLess>::tree_node_type* next)
    {
        m_next = next;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    inline
    typename Node<TCharType, TValueType, KeyCharLess>::tree_node_type*
    Node<TCharType, TValueType, KeyCharLess>::getNext() const
    {
        return m_next;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename Node<TCharType, TValueType, KeyCharLess>::tree_node_type*
    Node<TCharType, TValueType, KeyCharLess>::getBrother(TCharType keyChar, bool bCreateIfNotExist, bool& bCreated)
    {
        tree_node_type* destNode = nullptr;
        bCreated = false;

        if (isLess(keyChar, getKeyChar()))
        {
            tree_node_type* newNode = Node::clone(this);
            setNext(newNode);
            setChild(nullptr);

            setKeyChar(keyChar);
            setValue(get_undefined_value<TValueType>());

            destNode = this;

            bCreated = true;
        }

        // Если символ из ключа хранится в текущем элементе - вернем текущий элемент
        else if (isEqual(keyChar, getKeyChar()))
        {
            return this;
        }
        else
        {
            tree_node_type* nodePrev = this;
            tree_node_type* node = getNext();
            while (node && isLess(node->getKeyChar(), keyChar))
            {
                nodePrev = node;
                node = node->getNext();
            }

            if (node)
            {
                if (isEqual(node->getKeyChar(), keyChar))
                {
                    destNode = node;
                }
                else if (bCreateIfNotExist)
                {
                    tree_node_type* newNode = Node::create(keyChar);
                    tree_node_type* oldNext = nodePrev->getNext();
                    nodePrev->setNext(newNode);
                    newNode->setNext(oldNext);

                    destNode = newNode;

                    bCreated = true;
                }
            }
            else
            {
                if (bCreateIfNotExist)
                {
                    tree_node_type* newNode = Node::create(keyChar);
                    nodePrev->setNext(newNode);

                    destNode = newNode;

                    bCreated = true;
                }
            }
        }

        return destNode;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename Node<TCharType, TValueType, KeyCharLess>::tree_node_type*
    Node<TCharType, TValueType, KeyCharLess>::removeBrother(TCharType keyChar)
    {
        tree_node_type* result = nullptr;

        if (m_child && isEqual(m_child->getKeyChar(), keyChar))
        {
            result = m_child;
            m_child = m_next;
            m_next = m_next ? m_next->getNext() : nullptr;
        }
        else
        {
            tree_node_type* nodePrev = nullptr;
            for (tree_node_type* node = m_next; node != nullptr; node = node->getNext())
            {
                if (isEqual(node->getKeyChar(), keyChar))
                {
                    result = node;
                
                    if (nodePrev)
                        nodePrev->setNext(result->getNext());
                    else
                        m_next = result->getNext();

                    result->setNext(nullptr);

                    break;
                }

                nodePrev = node;
            }
        }

        return result;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    inline
    void
    Node<TCharType, TValueType, KeyCharLess>::setChild(typename Node<TCharType, TValueType, KeyCharLess>::tree_node_type* child)
    {
        m_child = child;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename Node<TCharType, TValueType, KeyCharLess>::tree_node_type*
     Node<TCharType, TValueType, KeyCharLess>::getChild() const
    {
        return m_child;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    inline
    typename Node<TCharType, TValueType, KeyCharLess>::tree_node_type*
    Node<TCharType, TValueType, KeyCharLess>::getChild(bool bCreateIfNotExist, bool& bCreated)
    {
        if (!m_child && bCreateIfNotExist)
        {
            m_child = Node::create();
            bCreated = true;
        }
        else
        {
            bCreated = false;
        }

        return m_child;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    inline
    bool
    Node<TCharType, TValueType, KeyCharLess>::isLess(TCharType ch1, TCharType ch2)
    {
        return ch1 == ch2 ? false : KeyCharLess()(ch1, ch2);
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    inline
    bool
    Node<TCharType, TValueType, KeyCharLess>::isEqual(TCharType ch1, TCharType ch2)
    {
        KeyCharLess less;
        return ch1 == ch2 ? true : (!less(ch1, ch2) && !less(ch2, ch1));
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    Trie<TCharType, TValueType, KeyCharLess>::Trie()
    {
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    Trie<TCharType, TValueType, KeyCharLess>::~Trie()
    {
        intResetRoot();
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    Node<TCharType, TValueType, KeyCharLess>*
    Trie<TCharType, TValueType, KeyCharLess>::addKeyValue(
        const string_type& key, TValueType value)
    {
        // Получим (и создадим недостающие элементы при необходимости) узел для указанного пути
        iterator_type it = intGetNode(key, key.length(), true);
        assert(it != end());

        Node<TCharType, TValueType, KeyCharLess>* result = *it;

        // Установим значение
        result->setValue(value);

        return result;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    bool
    Trie<TCharType, TValueType, KeyCharLess>::removeKey(const TrieStrings::StringOfChars<TCharType>& key)
    {
        bool bResult = false;

        if (0 == key.length())
        {
            intResetRoot();
            bResult = true;
        }

        // Получим узел для указанного пути без последнего символа
        else
        {
            const size_t index = key.length() - 1;
            iterator_type it = intGetNode(key, index, false);
            if (it != end())
            {
                Node<TCharType, TValueType, KeyCharLess>* result = *it;
                result = result->removeBrother(key.at(index));
                if (result)
                {
                    delete result;
                    bResult = true;
                }
            }
        }

        return bResult;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    TValueType
    Trie<TCharType, TValueType, KeyCharLess>::find(const TrieStrings::StringOfChars<TCharType>& key)
    {
        // Получим узел для указанного пути
        const_iterator_type it = intGetNode(key, key.length(), false);
        if (it != cend())
            return it->getValue();

        return get_undefined_value<TValueType>();
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename Trie<TCharType, TValueType, KeyCharLess>::const_iterator_type
    Trie<TCharType, TValueType, KeyCharLess>::lower_bound(const TrieStrings::StringOfChars<TCharType>& key) const
    {
        return intGetNode(key, key.length(), false);
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename Trie<TCharType, TValueType, KeyCharLess>::const_iterator_type
    Trie<TCharType, TValueType, KeyCharLess>::cbegin() const
    {
        auto it = const_iterator_type({ m_rootNode });
        ++it;

        return it;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename Trie<TCharType, TValueType, KeyCharLess>::const_iterator_type
    Trie<TCharType, TValueType, KeyCharLess>::cend() const
    {
        return const_iterator_type();
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename Trie<TCharType, TValueType, KeyCharLess>::iterator_type
    Trie<TCharType, TValueType, KeyCharLess>::begin() const
    {
        auto it = iterator_type({ m_rootNode });
        ++it;

        return it;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename Trie<TCharType, TValueType, KeyCharLess>::iterator_type
    Trie<TCharType, TValueType, KeyCharLess>::end() const
    {
        return iterator_type();
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename Trie<TCharType, TValueType, KeyCharLess>::node_type*
    Trie<TCharType, TValueType, KeyCharLess>::intGetRoot()
    {
        if (!m_rootNode)
            m_rootNode = Node<TCharType, TValueType, KeyCharLess>::create();

        return m_rootNode;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    void
    Trie<TCharType, TValueType, KeyCharLess>::intResetRoot()
    {
        delete m_rootNode;
        m_rootNode = nullptr;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename Trie<TCharType, TValueType, KeyCharLess>::iterator_type
    Trie<TCharType, TValueType, KeyCharLess>::intGetNode(
        const TrieStrings::StringOfChars<TCharType>& key, size_t keyLength, bool bCreateIfNotExist)
    {
        std::vector<node_type*> path;
        intGetNode(key, keyLength, path, bCreateIfNotExist);

        if (path.empty())
            return end();

        return iterator_type(path);
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    void
    Trie<TCharType, TValueType, KeyCharLess>::intGetNode(
        const TrieStrings::StringOfChars<TCharType>& key, size_t keyLength,
        std::vector<node_type*>& path,
        bool bCreateIfNotExist)
    {
        Node<TCharType, TValueType, KeyCharLess>* currentNode = intGetRoot();

        path.reserve(keyLength + 1);
        path.push_back(currentNode);

        for (size_t keyCharIndex = 0; keyCharIndex < keyLength; ++keyCharIndex)
        {
            const TCharType keyChar = key.at(keyCharIndex);

            bool bCreated(false);

            // Переходим на вложенный уровень
            currentNode = currentNode->getChild(bCreateIfNotExist, bCreated);
            if (!currentNode)
                break;

            // Проставим текущий символ ключа в только что созданный элемент
            if (bCreated)
                currentNode->setKeyChar(keyChar);

            // Создаем на вложенном уровне элемент для текущего символа ключа
            currentNode = currentNode->getBrother(keyChar, bCreateIfNotExist, bCreated);
            if (!currentNode)
                break;

            // Проставим текущий символ ключа в только что созданный элемент
            if (bCreated)
            {
                currentNode->setKeyChar(keyChar);
            }

            path.push_back(currentNode);
        }

        if (!currentNode)
            path.clear();
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    const_iterator<TCharType, TValueType, KeyCharLess>::const_iterator()
    {
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    const_iterator<TCharType, TValueType, KeyCharLess>::const_iterator(const std::vector<node_type*> nodes)
    {
        m_path.reserve(nodes.size());
        for (node_type* node : nodes)
        {
            TrieLevelInfo newPathItem;
            newPathItem.node = node;
            m_path.push_back(newPathItem);
        }
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    const_iterator<TCharType, TValueType, KeyCharLess>::const_iterator(
        const const_iterator<TCharType, TValueType, KeyCharLess>& other)
        : m_path(other.m_path)
    {
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    const_iterator<TCharType, TValueType, KeyCharLess>::const_iterator(
        const_iterator<TCharType, TValueType, KeyCharLess>&& other)
        : m_path(std::move(other.m_path))
    {
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    const typename const_iterator<TCharType, TValueType, KeyCharLess>::node_type*
    const_iterator<TCharType, TValueType, KeyCharLess>::operator->() const
    {
        return m_path.back().node;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    const typename const_iterator<TCharType, TValueType, KeyCharLess>::node_type*
    const_iterator<TCharType, TValueType, KeyCharLess>::operator*() const
    {
        return m_path.back().node;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename const const_iterator<TCharType, TValueType, KeyCharLess>::string_type&
    const_iterator<TCharType, TValueType, KeyCharLess>::getString() const
    {
        if (m_string.empty() && !m_path.empty())
        {
            m_string.reserve(m_path.size() + 1);

            for (const TrieLevelInfo& item : m_path)
            {
                // Проверим, что код символа отличен от нуля, т.к. в пути
                // первым элементом идет корень, в котором код символа есть ноль
                if (TCharType ch = item.node->getKeyChar())
                    m_string.appendChar(ch);
            }
        }

        return m_string;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    const TValueType&
    const_iterator<TCharType, TValueType, KeyCharLess>::getValue() const
    {
        const TrieLevelInfo& levelInfo = m_path.back();
        return levelInfo.node->getValue();
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    const_iterator<TCharType, TValueType, KeyCharLess>&
    const_iterator<TCharType, TValueType, KeyCharLess>::operator=(
        const const_iterator<TCharType, TValueType, KeyCharLess>& other)
    {
        m_path = other.m_path;

        return *this;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    const_iterator<TCharType, TValueType, KeyCharLess>&
    const_iterator<TCharType, TValueType, KeyCharLess>::operator=(
        const_iterator<TCharType, TValueType, KeyCharLess>&& other)
    {
        m_path = std::move(other);
        return *this;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    const_iterator<TCharType, TValueType, KeyCharLess>&
    const_iterator<TCharType, TValueType, KeyCharLess>::operator++()
    {
        if (m_path.empty())
            return *this;

        do
        {
            TrieLevelInfo& levelInfo = m_path.back();

            while (true)
            {
                if (levelInfo.bToChild)
                {
                    levelInfo.bToChild = false;

                    if (node_type* node = levelInfo.node->getChild())
                    {
                        m_path.push_back({ node });
                        m_string.clear();
                        break;
                    }
                }

                if (node_type* node = levelInfo.node->getNext())
                {
                    m_path.pop_back();
                    m_path.push_back({ node });
                    m_string.clear();
                    break;
                }

                // Переходим на уровень выше
                m_path.pop_back();
                m_string.clear();

                break;
            }

        } while (!m_path.empty() && m_path.back().node->haveValue());

        return *this;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    bool
    const_iterator<TCharType, TValueType, KeyCharLess>::operator==(const const_iterator& other) const
    {
        if (m_path.empty() && other.m_path.empty())
            return true;

        if (m_path.size() != other.m_path.size())
            return false;

        TrieLevelInfoVector::const_iterator itThis     = m_path.cbegin();
        TrieLevelInfoVector::const_iterator itEndThis  = m_path.cend();
        TrieLevelInfoVector::const_iterator itOther    = other.m_path.cbegin();
        TrieLevelInfoVector::const_iterator itEndOther = other.m_path.cend();
        for (; itThis != itEndThis; ++itThis, ++itOther)
        {
            if (itThis->node != itOther->node)
                return false;
        }

        return true;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    bool
    const_iterator<TCharType, TValueType, KeyCharLess>::operator!=(const const_iterator& other) const
    {
        return !operator==(other);
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    iterator<TCharType, TValueType, KeyCharLess>::iterator()
    {
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    iterator<TCharType, TValueType, KeyCharLess>::iterator(const std::vector<node_type*>& nodes)
        : const_iterator<TCharType, TValueType, KeyCharLess>(nodes)
    {
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    iterator<TCharType, TValueType, KeyCharLess>::iterator(const iterator& other)
        : const_iterator<TCharType, TValueType, KeyCharLess>(other)
    {
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    iterator<TCharType, TValueType, KeyCharLess>::iterator(iterator&& other)
        : const_iterator<TCharType, TValueType, KeyCharLess>(std::move(other))
    {
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename iterator<TCharType, TValueType, KeyCharLess>::node_type*
    iterator<TCharType, TValueType, KeyCharLess>::operator->() const
    {
        return const_iterator<TCharType, TValueType, KeyCharLess>::m_path.back().node;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    typename iterator<TCharType, TValueType, KeyCharLess>::node_type*
    iterator<TCharType, TValueType, KeyCharLess>::operator*() const
    {
        return const_iterator<TCharType, TValueType, KeyCharLess>::m_path.back().node;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    iterator<TCharType, TValueType, KeyCharLess>&
    iterator<TCharType, TValueType, KeyCharLess>::operator=(const iterator& other)
    {
        const_iterator::operator=(other);
        return *this;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    iterator<TCharType, TValueType, KeyCharLess>&
    iterator<TCharType, TValueType, KeyCharLess>::operator=(iterator&& other)
    {
        const_iterator::operator=(std::move(other));
        return *this;
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    bool
    iterator<TCharType, TValueType, KeyCharLess>::operator==(const iterator& other) const
    {
        return const_iterator<TCharType, TValueType, KeyCharLess>::operator==(other);
    }

    //------------------------------------------------------------------------//
    template<typename TCharType, typename TValueType, typename KeyCharLess>
    bool
    iterator<TCharType, TValueType, KeyCharLess>::operator!=(const iterator& other) const
    {
        return const_iterator<TCharType, TValueType, KeyCharLess>::operator!=(other);
    }

    //------------------------------------------------------------------------//
}
