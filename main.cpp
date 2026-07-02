// Customer Review Recommendation Engine
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <limits>
#include <cmath>
using namespace std;

// Layer 1 

// Data Models

// making required structs for the project

struct PurchaseRecord {
    int productId;
    float rating;
    string reviewText;
};

struct User {
    int userId;
    string name, email;
    vector<int> purchasedProducts;
    unordered_map<int, PurchaseRecord> reviewMap;  // productId -> review
};

struct Product {
    int productId;
    string name, category;
    float price, avgRating;
    int totalReviews;
    vector<float> allRatings;
};

struct Edge {
    int toId;
    float weight;
};

struct Graph {
    unordered_map<int, vector<Edge>> userAdj;         // user-user
    unordered_map<int, vector<Edge>> productAdj;      // product-product
    unordered_map<int, vector<Edge>> userProductAdj;  // user -> products
};

// Making Helper Functions for Users and Products

// function to register purchase by an user 
void userAddPurchase(User& u, int pid, float rating, const string& review) {
    u.purchasedProducts.push_back(pid);
    PurchaseRecord rec;
    rec.productId  = pid;
    rec.rating     = rating;
    rec.reviewText = review;
    u.reviewMap[pid] = rec;
}

// function to check if user has bought a required produt
bool userHasBought(const User& u, int pid) {
    return u.reviewMap.count(pid) > 0;
}

// this function returns the review given by the user to the product if bought
float userGetRating(const User& u, int pid) {
    if (userHasBought(u, pid)) return u.reviewMap.at(pid).rating;
    return -1.0f;
}

// this function returns number of common product of two users 
int usersCommonProducts(const User& u1, const User& u2) {
    int cnt = 0;
    for (int pid : u1.purchasedProducts)
        if (userHasBought(u2, pid)) cnt++;
    return cnt;
}

// this functions adds new rating to the product and recomputes its average 
void productAddRating(Product& p, float r) {
    p.allRatings.push_back(r);
    p.totalReviews++;
    float sum = 0;
    for (float x : p.allRatings) sum += x;
    p.avgRating = sum / p.totalReviews;
}

// this functions rerturns a score from 0 to 1 depending on how much 2 products are similar
// important formula used here is similarity = categoryScore +  (1 - |avgRating1 - avgRating2| / 5) * 0.5
float productSimilarity(const Product& p1, const Product& p2) {
    float score = (p1.category == p2.category) ? 0.5f : 0.0f; // this is the category score
    score += (1.0f - fabs(p1.avgRating - p2.avgRating) / 5.0f) * 0.5f;
    return score;
}

// Loading Data for ananlysing

// this helps in splitting lines of data into deliverable 
// here maxparts ensure line is spliited only in 4 parts
vector<string> splitLine(const string& line, char c, int maxParts) {
    vector<string> parts;
    string cur;
    for (char ch : line) {
        if (ch == c && (int)parts.size() < maxParts - 1) { // breaks at a comma
            parts.push_back(cur);
            cur = "";
        } else {
            cur += ch;
        }
    }
    parts.push_back(cur);
    return parts;
}

// this function loads all users from a file and stores them in a hashmap
unordered_map<int, User> loadUsers(const string& filename) {
    unordered_map<int, User> users;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto p = splitLine(line, ',', 3); // this splits line into 3 parts by using commma as a seperator
        if (p.size() < 3) continue;
        User u;
        u.userId = stoi(p[0]);
        u.name   = p[1];
        u.email  = p[2];
        users[u.userId] = u;
    }
    cout << "We have successfully loaded " << users.size() << " users."<<endl;
    return users;
}

// this function also loads all products from a file and then stores them in a hash map 
// similar to what is done for the users already
unordered_map<int, Product> loadProducts(const string& filename) {
    unordered_map<int, Product> products;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto p = splitLine(line, ',', 4); // // this splits line into 4 parts by using commma as a seperator
        if (p.size() < 4) continue;
        Product prod;
        prod.productId   = stoi(p[0]);
        prod.name        = p[1];
        prod.category    = p[2];
        prod.price       = stof(p[3]);
        prod.avgRating   = 0.0f;
        prod.totalReviews = 0;
        products[prod.productId] = prod;
    }
    cout << "We have successfully loaded " << products.size() << " products."<<endl;
    return products;
}

// this function loads reviews and connects with the products and users
void loadReviews(const string& filename,unordered_map<int, User>& users, unordered_map<int, Product>& products) {
    ifstream file(filename);
    if (!file.is_open()) { cout << "ERROR: can't open " << filename << "\n"; return; } // gives error is we cant open file
    int count = 0;
    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto p = splitLine(line, ',', 4); // splits line into 4 parts using comms as a seperator
        if (p.size() < 4) continue;
        int uid = stoi(p[0]), pid = stoi(p[1]);
        float rating = stof(p[2]);
        if (users.count(uid) && products.count(pid)) {
            userAddPurchase(users[uid], pid, rating, p[3]); // adds rating to the user's hashmap of review and productID
            productAddRating(products[pid], rating); // does similar thing for products 
            count++;
        }
    }
    cout << "We have successfully loaded " << count << " reviews.\n";
}

// Layer 2 Graph Construction

// this function build a graph connecting similar users
void buildUserGraph(Graph& g, unordered_map<int, User>& users) {
    for (auto& [id1, u1] : users) {
        for (auto& [id2, u2] : users) {
            if (id1 >= id2) continue;
            int common = usersCommonProducts(u1, u2); // finding out common products between 2 users
            if (common == 0) continue;
            float w = (float)common;
            for (int pid : u1.purchasedProducts)
                if (userHasBought(u2, pid))
                    w += (1.0f - fabs(userGetRating(u1, pid) - userGetRating(u2, pid)) / 5.0f) * 0.5f;
                    // making a function to determine the weight of the edge similar to the one used above in product similarity
            // making a bidirectional graph
            g.userAdj[id1].push_back({id2, w});
            g.userAdj[id2].push_back({id1, w});
        }
    }
    cout << "User graph has now been built. " << g.userAdj.size() << " Users are also connected.\n";
}

// this function build a graph connecting similar products 
void buildProductGraph(Graph& g, unordered_map<int, Product>& products) {
    for (auto& [id1, p1] : products) {
        for (auto& [id2, p2] : products) {
            if (id1 >= id2) continue;
            float sim = productSimilarity(p1, p2); // this returns the value from prosuct similaity function
            if (sim < 0.3f) continue; // products with weak similarity are skipped 
            g.productAdj[id1].push_back({id2, sim}) ;
            g.productAdj[id2].push_back({id1, sim});
        }
    }
    cout << "Product graph has now been built. " << g.productAdj.size() << "Products are also connected.\n";
}

// this function builds connections between users and the products they purchased
void buildUserProductGraph(Graph& g, unordered_map<int, User>& users) {
    for (auto& [uid, user] : users)
        for (int pid : user.purchasedProducts)
            g.userProductAdj[uid].push_back({pid, userGetRating(user, pid)}); // this createss user and product graph with weight or rating  as an edge
    cout << "User-product graph built.\n";
}

// this function builds all graphs needed in the recommendation system
void buildAllGraphs(Graph& g,
                    unordered_map<int, User>& users,
                    unordered_map<int, Product>& products) {
    buildUserGraph(g, users);
    buildProductGraph(g, products);
    buildUserProductGraph(g, users);
}

// this displays all connected users to a given user
void displayUserEdges(int uid, Graph& g) {
    cout << "User " << uid << " connections:\n";
    if (!g.userAdj.count(uid)) { cout << "  None.\n"; return; }
    for (auto& e : g.userAdj[uid])
        cout << "  -> User " << e.toId << " | weight: " << e.weight << "\n";
}

// this displays all products connected to a given product
void displayProductEdges(int pid, Graph& g) {
    cout << "Product " << pid << " connections:\n";
    if (!g.productAdj.count(pid)) { cout << "  None.\n"; return; }
    for (auto& e : g.productAdj[pid])
        cout << "  -> Product " << e.toId << " | similarity: " << e.weight << "\n";
}

// Layer 3 Algorithms 

// Dijkstra Algorithm

// this function finds shortest similarity distance from one user to all other users using dijkstra
unordered_map<int, float> dijkstraUsers(int srcId, Graph& g) {
    unordered_map<int, float> dist;
    for (auto& [uid, _] : g.userAdj)
        dist[uid] = numeric_limits<float>::infinity();
    dist[srcId] = 0.0f;

    priority_queue<pair<float,int>, vector<pair<float,int>>, greater<>> pq; // making priority queue
    pq.push({0.0f, srcId});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u] || !g.userAdj.count(u)) continue;
        for (auto& e : g.userAdj[u]) {
            float newDist = dist[u] + 1.0f / (e.weight + 0.001f); // here inverse is taken higher similarity should mean smaller distance
            if (newDist < dist[e.toId]) {
                dist[e.toId] = newDist;
                pq.push({newDist, e.toId});
            }
        }
    }
    return dist;
}

// This function finds and returns users most similar to a given user along with their similarity distances
vector<pair<int,float>> getSimilarUsers(int srcId, Graph& g) {
    auto dist = dijkstraUsers(srcId, g);
    vector<pair<int,float>> result;
    for (auto& [uid, d] : dist)
        if (uid != srcId && d < numeric_limits<float>::infinity())
            result.push_back({uid, d});
    sort(result.begin(), result.end(), [](auto& a, auto& b){ return a.second < b.second; });
    return result;
}

// this function displays similar users 
void displaySimilarUsers(int userId, Graph& g) {
    cout << "Users most similar to User " << userId << ":\n";
    for (auto& [uid, d] : getSimilarUsers(userId, g))
        cout << "  User " << uid << " | distance: " << d << "\n";
}

// Breadth First Search
// In general what dijkstra algo did for users bfs will do for products 
// we used djikstra there because users needed weight but not products need connected components

// this function finds nearby similar products using bfs traversal
unordered_map<int,int> bfsSimilarProducts(int srcPid, Graph& g, int maxHops = 2) {
    unordered_map<int,int> visited;
    queue<pair<int,int>> q;
    q.push({srcPid, 0});
    visited[srcPid] = 0;

    while (!q.empty()) {
        auto [curr, hops] = q.front(); q.pop();
        if (hops >= maxHops || !g.productAdj.count(curr)) continue;  // stopping traversal if max hops reached
        for (auto& e : g.productAdj[curr]) {
            if (!visited.count(e.toId)) {
                visited[e.toId] = hops + 1;
                q.push({e.toId, hops + 1});
            }
        }
    }
    visited.erase(srcPid);
    return visited;
}

// This function gets and returns similar products sorted by nearest hop distance
vector<pair<int,int>> getSimilarProducts(int srcPid, Graph& g, int maxHops = 2) {
    auto v = bfsSimilarProducts(srcPid, g, maxHops);
    vector<pair<int,int>> result(v.begin(), v.end());
    sort(result.begin(), result.end(), [](auto& a, auto& b){ return a.second < b.second; });
    return result;
}

// this function displays similar products
void displaySimilarProducts(int pid, Graph& g, unordered_map<int, Product>& products, int maxHops = 2) {
    cout << "Products similar to Product " << pid;
    if (products.count(pid)) cout << " (" << products[pid].name << ")";
    cout << " within " << maxHops << " hops:\n";
    for (auto& [simPid, hops] : getSimilarProducts(pid, g, maxHops)) {
        cout << "  [hop " << hops << "] Product " << simPid;
        if (products.count(simPid)) cout << " - " << products[simPid].name;
        cout << "\n";
    }
}

// Layer 4 Recommender

vector<pair<int,float>> recommend(int userId, int topK,Graph& g,unordered_map<int, User>& users,unordered_map<int, Product>& products) {
    // this creates a hashmap to store recommendation scores
    if (!users.count(userId)) { cout << "User " << userId << " not found.\n"; return {}; }
    User& target = users[userId];
    unordered_map<int,float> scores;

    // Signal 1  Collaborative Filtering (Dijkstra)
    // This code recommends products liked by similar users 
    // it also increases score based on similarity and ratings.
    for (auto& [simUid, dist] : getSimilarUsers(userId, g)) {
        if (!users.count(simUid)) continue;
        float similarity = 1.0f / (dist + 0.001f);
        for (int pid : users[simUid].purchasedProducts) {
            if (userHasBought(target, pid)) continue;
            scores[pid] += similarity * userGetRating(users[simUid], pid) * 0.5f;
        }
    }

    // Signal 2 Content-Based Filtering (BFS)
    // This code recommends products similar to products that the user already bought
    for (int pid : target.purchasedProducts) {
        float userRating = userGetRating(target, pid);
        if (userRating < 3.5f) continue;
        for (auto& [simPid, hops] : getSimilarProducts(pid, g, 2)) {
            if (userHasBought(target, simPid)) continue;
            float hopDecay = (hops == 1) ? 1.0f : 0.5f;
            scores[simPid] += userRating * hopDecay * 0.5f;
        }
    }

    // This code stores products in descending order of recommendation quality
    priority_queue<pair<float,int>> maxHeap;
    for (auto& [pid, score] : scores) {
        if (!products.count(pid)) continue;
        maxHeap.push({score + products[pid].avgRating * 0.1f, pid});
    }

    // this part selects top recommended products
    vector<pair<int,float>> result;
    while (!maxHeap.empty() && (int)result.size() < topK) {
        auto [score, pid] = maxHeap.top(); maxHeap.pop();
        result.push_back({pid, score});
    }
    return result;
}

// it helps to display the recommendations by the algorithms
void displayRecommendations(int userId, int topK,Graph& g,unordered_map<int, User>& users,unordered_map<int, Product>& products) {
    cout << "  Recommendations for User " << userId;
    if (users.count(userId)) cout << " (" << users[userId].name << ")"<<endl<<endl;

    if (users.count(userId)) {
        cout << "Already purchased: ";
        for (int pid : users[userId].purchasedProducts)
            if (products.count(pid)) cout << products[pid].name << "  ";
        cout << "\n\n";
    }

    auto recs = recommend(userId, topK, g, users, products);
    if (recs.empty()) { cout << "No recommendations found.\n"; return; }

    cout << "Top " << topK << " recommendations:\n";
    int rank = 1;
    for (auto& [pid, score] : recs)
        if (products.count(pid))
            cout << "  " << rank++ << ". " << products[pid].name
                 << " | " << products[pid].category
                 << " | Avg Rating: " << products[pid].avgRating
                 << " | Score: " << score << "\n";
}

// it helps to display what similar users also bought online
void displayCollaborative(int userId,
                           Graph& g,
                           unordered_map<int, User>& users,
                           unordered_map<int, Product>& products) {
    cout << "--- Users like you also bought ---\n";
    unordered_set<int> owned;
    if (users.count(userId))
        for (int pid : users[userId].purchasedProducts) owned.insert(pid);

    unordered_map<int,int> freq;
    for (auto& [simUid, _] : getSimilarUsers(userId, g))
        if (users.count(simUid))
            for (int pid : users[simUid].purchasedProducts)
                if (!owned.count(pid)) freq[pid]++;

    priority_queue<pair<int,int>> pq;
    for (auto& [pid, cnt] : freq) pq.push({cnt, pid});

    int shown = 0;
    while (!pq.empty() && shown < 3) {
        auto [cnt, pid] = pq.top(); pq.pop();
        if (products.count(pid))
            cout << "  " << products[pid].name << " (bought by " << cnt << " similar users)\n";
        shown++;
    }
}

// Main 

int main() {
    cout << "\n  Customer Review Recommendation Engine\n\n";

    // Layer 1: Load Data
    cout << " Layer 1: Data Loading \n";
    auto users    = loadUsers("data/users.txt");
    auto products = loadProducts("data/products.txt");
    loadReviews("data/reviews.txt", users, products);

    // Layer 2: Build Graphs
    cout << "\n Layer 2: Graph Construction \n";
    Graph graph;
    buildAllGraphs(graph, users, products);
    cout << "\n";
    displayUserEdges(1, graph);
    cout << "\n";
    displayProductEdges(101, graph);

    // Layer 3: Algorithms
    cout << "\n Layer 3: Algorithms \n";
    displaySimilarUsers(1, graph);
    cout << "\n";
    displaySimilarProducts(101, graph, products, 2);

    // Layer 4: Recommendations
    cout << "\n Layer 4: Recommendations\n";
    for (int uid = 1; uid <= 5; uid++) {
        displayRecommendations(uid, 3, graph, users, products);
        displayCollaborative(uid, graph, users, products);
        cout << "\n";
    }

    return 0;
}