# 🛍️ Customer Review Recommendation Engine

> A DSA course project implementing a product recommendation system in C++ using graphs, Dijkstra's algorithm, and BFS.

---

## 📌 Overview

This project simulates a real-world product recommendation engine — similar to what you'd find on Amazon or Flipkart — built entirely from scratch using core Data Structures and Algorithms concepts. It takes user purchase history and ratings as input, builds weighted graphs connecting users and products, and uses graph traversal algorithms to generate personalized recommendations.

---

## 🏗️ Architecture — 4-Layer Design

The system is organized into four distinct layers:

### Layer 1 — Data Models & Loading
Defines the core data structures and loads data from flat files:
- `User` — stores user info, purchased products, and a review map (`unordered_map`)
- `Product` — stores product details, all ratings, and a computed average rating
- `PurchaseRecord` — links a user's rating and review text to a product
- Data is loaded from `data/users.txt`, `data/products.txt`, and `data/reviews.txt`

### Layer 2 — Graph Construction
Builds three separate graphs from the loaded data:
- **User–User Graph** — connects users who share common purchases; edge weight reflects the number of shared products and how similarly they rated them
- **Product–Product Graph** — connects products by a similarity score based on category match and average rating closeness
- **User–Product Graph** — connects each user to the products they purchased, with the edge weight being the rating given

### Layer 3 — Algorithms
Two graph algorithms power the similarity computations:
- **Dijkstra's Algorithm** (on the User–User graph) — finds the "closest" users to a given user by treating high similarity as low distance (using inverse of weight)
- **BFS** (on the Product–Product graph) — finds products similar to a given product within a configurable hop limit

### Layer 4 — Recommendation Engine
Combines two classical recommendation strategies:
- **Collaborative Filtering** — recommends products that similar users (found via Dijkstra) have rated highly but the target user hasn't bought yet
- **Content-Based Filtering** — recommends products similar (found via BFS) to items the user already rated ≥ 3.5
- A max-heap (`priority_queue`) is used to extract the top-K recommendations efficiently

---

## 🧠 DSA Concepts Used

| Concept | Where Used |
|---|---|
| `unordered_map` (Hash Map) | User registry, product registry, review lookup, adjacency lists |
| `vector` | Adjacency lists, rating history, purchase lists |
| Weighted Graph | User–User and Product–Product similarity graphs |
| **Dijkstra's Algorithm** | Finding most similar users via shortest path |
| **BFS** | Finding similar products within N hops |
| `priority_queue` (Max-Heap) | Extracting top-K recommendations by score |
| Struct-based OOP | Clean data modelling with `User`, `Product`, `Edge`, `Graph` |

---

## 📁 Project Structure

```
Recomendation_Engine/
├── main.cpp          # Full source code (~500 lines, well-commented)
├── Makefile          # Build configuration
└── data/
    ├── users.txt     # User records (id, name, email)
    ├── products.txt  # Product catalogue (id, name, category, price)
    └── reviews.txt   # Purchase & review data (userId, productId, rating, review)
```

---

## 🗂️ Data Format

**`data/users.txt`**
```
# userId,name,email
1,Arjun Sharma,arjun@gmail.com
```

**`data/products.txt`**
```
# productId,name,category,price
101,Samsung Galaxy S23,Electronics,79999
```

**`data/reviews.txt`**
```
# userId,productId,rating,timestamp,reviewText
1,101,4.5,1700000100,Amazing camera and battery life
```

---

## ⚙️ Build & Run

**Requirements:** g++ with C++17 support

```bash
# Build
make

# Run
./engine

# Build and run in one step
make run

# Clean build artifacts
make clean
```

---

## 📊 Sample Output

```
Layer 1: Data Loading
We have successfully loaded 5 users.
We have successfully loaded 10 products.
We have successfully loaded 15 reviews.

Layer 2: Graph Construction
User graph has now been built.
Product graph has now been built.

Layer 3: Algorithms
Users most similar to User 1:
  User 2 | distance: 0.312
  User 3 | distance: 0.476

Layer 4: Recommendations
Recommendations for User 1 (Arjun Sharma)
Already purchased: Samsung Galaxy S23  Sony WH-1000XM5  Introduction to Algorithms

Top 3 recommendations:
  1. OnePlus 11 | Electronics | Avg Rating: 4.25 | Score: 3.84
  2. Atomic Habits | Books | Avg Rating: 4.50 | Score: 2.91
  3. Clean Code | Books | Avg Rating: 4.50 | Score: 2.10
```

---

## 🔑 Key Formulas

**User–User Edge Weight:**
```
weight = commonProducts + Σ (1 - |rating1 - rating2| / 5) × 0.5
```

**Product Similarity Score:**
```
similarity = categoryScore (0.5 if same, else 0) + (1 - |avgRating1 - avgRating2| / 5) × 0.5
```

**Dijkstra Distance (Inverse Similarity):**
```
distance = 1 / (edgeWeight + 0.001)
```
Higher similarity → smaller distance → prioritized by the algorithm.

---

## 🚀 Possible Extensions

- Load larger real-world datasets (e.g., from Kaggle)
- Add a CLI/menu for interactive user queries
- Implement matrix factorization or cosine similarity for richer collaborative filtering
- Persist the graph to disk for faster startup on large datasets
- Add a web front-end or REST API wrapper

---

## 👤 Author

**DSA Course Project**  
Presented with video demonstration and PowerPoint slides.

---

## 📄 License

For academic/educational use only.
