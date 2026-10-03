# 🛍️ Customer Review Recommendation Engine

A C++ recommendation engine built from scratch using **Data Structures and Algorithms**. The system models users, products, purchases, and ratings as graphs and combines **collaborative filtering** with **content-based filtering** to generate personalized product recommendations.

The project is designed as an educational implementation of how a recommendation system can be built using fundamental graph algorithms and data structures rather than external machine-learning libraries.

---

## ✨ Features

- 👤 User and product data management
- ⭐ Product ratings and review storage
- 🔗 User–User similarity graph
- 🛍️ Product–Product similarity graph
- 🔄 User–Product interaction graph
- 🧭 Dijkstra's algorithm for finding similar users
- 🌐 BFS for finding similar products
- 🤝 Collaborative filtering
- 🎯 Content-based filtering
- 🏆 Hybrid recommendation scoring
- 🥇 Top-K recommendation retrieval using a max-heap
- 📁 File-based data loading
- ⚡ C++17 implementation with a Makefile

---

## 🧠 How It Works

The system follows a four-layer architecture.

### Layer 1 — Data Loading

The engine loads three datasets:

- `users.txt` — user information
- `products.txt` — product catalogue
- `reviews.txt` — purchases, ratings, and review text

The data is stored using structures such as:

- `User`
- `Product`
- `PurchaseRecord`
- `Edge`
- `Graph`

Hash maps (`unordered_map`) provide fast access to users, products, and review information.

---

### Layer 2 — Graph Construction

Three graphs are constructed from the loaded data.

#### 1. User–User Graph

Two users are connected when they have purchased at least one common product.

The edge weight considers:

- Number of common products
- Similarity between their ratings of common products

This graph represents **user behavioral similarity**.

#### 2. Product–Product Graph

Products are connected based on their similarity.

The similarity score considers:

- Whether they belong to the same category
- How close their average ratings are

Only sufficiently similar products are connected.

#### 3. User–Product Graph

Each user is connected to the products they purchased.

The edge weight represents the user's rating for that product.

---

## 🔬 Algorithms

### Dijkstra's Algorithm — Similar Users

Dijkstra's algorithm is applied to the User–User graph.

Because a larger edge weight represents greater similarity, the implementation converts similarity into distance:

```text
distance = 1 / (edgeWeight + 0.001)
```

Therefore:

```text
Higher similarity → Smaller distance
```

This allows Dijkstra's shortest-path algorithm to identify users who are behaviorally close to the target user.

---

### BFS — Similar Products

Breadth-First Search is applied to the Product–Product graph.

The search can be limited by the maximum number of hops. The recommendation engine currently searches up to **2 hops**.

```text
Product A
   │
   ├── Product B        (1 hop)
   │       │
   │       └── Product C (2 hops)
```

Products closer to the original product receive a stronger contribution to the recommendation score.

---

## 🎯 Recommendation Engine

The final recommendation system combines two signals.

### 1. Collaborative Filtering

Products purchased by users similar to the target user are considered.

The score increases when:

- The recommending user is more similar to the target user
- The recommending user gave the product a higher rating

Products already purchased by the target user are excluded.

### 2. Content-Based Filtering

Products similar to items the target user already liked are considered.

Only products rated **3.5 or higher** by the target user are used as seeds.

Products at:

- **1 hop** receive full contribution
- **2 hops** receive a reduced contribution

### 3. Global Rating Signal

The product's average rating is also included as a small additional signal.

The resulting candidates are stored in a `priority_queue` and the highest-scoring products are returned as the **Top-K recommendations**.

---

## 📐 Core Formulas

### User Similarity

For two users:

```text
weight =
    commonProducts
    + Σ [(1 - |rating1 - rating2| / 5) × 0.5]
```

A higher value indicates stronger similarity.

### Product Similarity

```text
similarity =
    categoryScore
    + (1 - |avgRating1 - avgRating2| / 5) × 0.5
```

where:

```text
categoryScore = 0.5  if categories are the same
                0.0  otherwise
```

### Dijkstra Distance

```text
distance = 1 / (edgeWeight + 0.001)
```

This converts a similarity weight into a distance suitable for shortest-path algorithms.

---

## 📁 Project Structure

```text
Recomendation_Engine/
│
├── main.cpp
├── Makefile
├── README.md
│
└── data/
    ├── users.txt
    ├── products.txt
    └── reviews.txt
```

### File Responsibilities

| File | Purpose |
|---|---|
| `main.cpp` | Complete recommendation engine implementation |
| `Makefile` | Build, run, and clean commands |
| `data/users.txt` | User records |
| `data/products.txt` | Product catalogue |
| `data/reviews.txt` | Purchase and rating data |

---

## 📊 Data Format

### `data/users.txt`

```text
# userId,name,email
1,Arjun Sharma,arjun@gmail.com
2,Riya Singh,riya@gmail.com
```

### `data/products.txt`

```text
# productId,name,category,price
101,Samsung Galaxy S23,Electronics,79999
102,OnePlus 11,Electronics,59999
103,Atomic Habits,Books,499
```

### `data/reviews.txt`

```text
# userId,productId,rating,reviewText
1,101,4.5,Amazing camera and battery life
1,103,5.0,Very useful and practical book
2,101,4.0,Good overall smartphone
```

> **Note:** Review text is stored by the program but is not currently used as an NLP feature in the recommendation score.

---

## ⚙️ Requirements

- **C++17** or later
- `g++`
- `make`

No external libraries are required.

---

## 🚀 Installation & Usage

### 1. Clone the repository

```bash
git clone https://github.com/crazyguy96/Recomendation_Engine.git
cd Recomendation_Engine
```

### 2. Build the project

```bash
make
```

This compiles the program using:

```text
g++ -std=c++17 -Wall -O2
```

and creates the executable:

```text
engine
```

### 3. Run

```bash
./engine
```

Or build and run in one command:

```bash
make run
```

### 4. Clean

```bash
make clean
```

---

## 🔄 Execution Pipeline

```text
             ┌──────────────────┐
             │   Input Files    │
             │ users/products/  │
             │     reviews      │
             └────────┬─────────┘
                      │
                      ▼
             ┌──────────────────┐
             │   Data Models    │
             │ User / Product   │
             │ Review Records   │
             └────────┬─────────┘
                      │
                      ▼
             ┌──────────────────┐
             │ Graph Construction│
             │                  │
             │ User ↔ User      │
             │ Product ↔ Product│
             │ User ↔ Product   │
             └────────┬─────────┘
                      │
             ┌────────┴─────────┐
             ▼                  ▼
      ┌──────────────┐   ┌──────────────┐
      │   Dijkstra   │   │     BFS      │
      │ Similar Users│   │ Similar Items│
      └──────┬───────┘   └──────┬───────┘
             │                  │
             └────────┬─────────┘
                      ▼
             ┌──────────────────┐
             │ Hybrid Scoring   │
             │ Collaborative +  │
             │ Content-Based    │
             └────────┬─────────┘
                      │
                      ▼
             ┌──────────────────┐
             │   Max Heap       │
             │     Top-K        │
             └────────┬─────────┘
                      │
                      ▼
             ┌──────────────────┐
             │ Recommendations  │
             └──────────────────┘
```

---

## 🧰 Data Structures Used

| Data Structure | Usage |
|---|---|
| `unordered_map` | Users, products, reviews, adjacency lists |
| `unordered_set` | Tracking already purchased products |
| `vector` | Purchases, ratings, adjacency lists |
| `queue` | BFS traversal |
| `priority_queue` | Dijkstra priority queue and Top-K recommendations |
| Weighted Graph | User and product similarity modelling |
| `struct` | Core data models |

---

## 🧪 Example Workflow

For a target user:

```text
User 1
│
├── Previously purchased:
│   ├── Product A ⭐ 5.0
│   ├── Product B ⭐ 4.5
│   └── Product C ⭐ 2.0
│
├── Find similar users using Dijkstra
│
├── Find similar products to highly-rated items using BFS
│
├── Combine recommendation signals
│
└── Return Top-K unseen products
```

The program also reports similar users, similar products, personalized recommendations, and products frequently purchased by similar users.

---

## 📚 DSA Concepts Demonstrated

This project focuses on practical applications of core DSA concepts:

- Hashing
- Graph representation
- Weighted graphs
- Graph traversal
- Shortest-path algorithms
- Breadth-First Search
- Priority queues / heaps
- Sorting
- Adjacency lists
- Data modelling with structures

---

## 🚧 Limitations

This is an educational recommendation engine rather than a production-scale recommender.

Current limitations include:

- Small file-based datasets
- No persistent database
- No real-time user feedback loop
- Review text is not analyzed using NLP
- Product similarity uses a simple hand-designed formula
- No matrix factorization or embedding-based recommendations
- Graph construction can become expensive as the number of users/products grows
- No web interface or REST API

---

## 🔮 Future Improvements

Possible extensions include:

- 📈 Support larger real-world datasets
- 🧠 Add NLP-based review analysis
- 🤖 Add matrix factorization
- 📐 Add cosine similarity
- 🔥 Add more sophisticated hybrid recommendation models
- 💾 Persist precomputed graphs
- 🌐 Build a web interface
- 🔌 Expose the engine through a REST API
- ⚡ Optimize graph construction for large datasets
- 👤 Add an interactive CLI for selecting users and products
- 📊 Add recommendation evaluation metrics such as Precision@K and Recall@K

---

## 🎓 Project Context

This project demonstrates how fundamental **Data Structures and Algorithms** can be applied to a practical recommendation-system problem.

Instead of relying on a machine-learning framework, the core recommendation pipeline is implemented manually using C++ data structures, graph construction, Dijkstra's algorithm, BFS, and priority queues.

---

## 👤 Author

**crazyguy96**

Repository:  
https://github.com/crazyguy96/Recomendation_Engine

---

## 📄 License

This project is intended primarily for **academic and educational use**.
