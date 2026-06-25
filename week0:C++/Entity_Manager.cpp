#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Entity
{
private:
    int id;
    std::string name;

public:
    Entity(int id, const std::string& name)
        : id(id), name(name){}

    ~Entity()
    {
        std::cout << "Destroying Entity: " << name << '\n';
    }

    int getId() const
    {
        return id;
    }

    const std::string& getName() const
    {
        return name;
    }

    friend std::ostream& operator<<(std::ostream& os,
                                    const Entity& entity);
};

std::ostream& operator<<(std::ostream& os,
                         const Entity& entity)
{
    os << "Entity{id="
       << entity.id
       << ", name="
       << entity.name
       << "}";

    return os;
}

class EntityManager
{
private:
    std::vector<std::unique_ptr<Entity>> entities;

public:
    void addEntity(int id, const std::string& name)
    {
        if (find(id) != nullptr)
        {
            std::cout << "Entity with ID "
                      << id
                      << " already exists.\n";
            return;
        }

        entities.push_back(
            std::make_unique<Entity>(id, name));
    }

    Entity* find(int id)
    {
        for (auto& entity : entities)
        {
            if (entity->getId() == id)
            {
                return entity.get();
            }
        }

        return nullptr;
    }

    Entity* find(const std::string& name)
    {
        for (auto& entity : entities)
        {
            if (entity->getName() == name)
            {
                return entity.get();
            }
        }

        return nullptr;
    }

    void removeEntity(int id)
    {
        entities.erase(
            std::remove_if(
                entities.begin(),
                entities.end(),
                [id](const std::unique_ptr<Entity>& entity)
                {
                    return entity->getId() == id;
                }),
            entities.end());
    }

    void printAll() const
    {
        for (const auto& entity : entities)
        {
            std::cout << *entity << '\n';
        }
    }

    void sortById()
    {
        std::sort(
            entities.begin(),
            entities.end(),
            [](const auto& a, const auto& b)
            {
                return a->getId() < b->getId();
            });
    }
};

template<typename T>
void printVector(const std::vector<T>& values)
{
    for (const auto& value : values)
    {
        std::cout << value << '\n';
    }
}

int main()
{
    EntityManager manager;

    manager.addEntity(3, "NPC");
    manager.addEntity(1, "Player");
    manager.addEntity(2, "Enemy");

    std::cout << "Initial entities:\n";
    manager.printAll();

    std::cout << "\nSearching by ID:\n";

    if (auto* entity = manager.find(2))
    {
        std::cout << "Found: "
                  << *entity
                  << '\n';
    }

    std::cout << "\nSearching by name:\n";

    if (auto* entity = manager.find("Player"))
    {
        std::cout << "Found: "
                  << *entity
                  << '\n';
    }

    std::cout << "\nSorting by ID:\n";
    manager.sortById();
    manager.printAll();

    std::cout << "\nRemoving entity with ID 2:\n";
    manager.removeEntity(2);
    manager.printAll();

    std::cout << "\nProgram ending...\n";

    return 0;
}