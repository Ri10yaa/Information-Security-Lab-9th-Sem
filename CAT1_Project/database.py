"""
database.py — SQLite setup and seed data for Security Attack Demo
Intentionally stores plaintext passwords for SQL Injection demonstration.
"""

import sqlite3
import os

DB_PATH = os.path.join(os.path.dirname(__file__), 'demo.db')


def get_db():
    """Return a raw SQLite connection."""
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn


def init_db():
    """Create tables and seed demo data."""
    conn = get_db()
    cur = conn.cursor()

    # ── Users table (for SQL Injection demo) ─────────────────────────────────
    cur.execute("DROP TABLE IF EXISTS users")
    cur.execute("""
        CREATE TABLE users (
            id       INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL,
            password TEXT NOT NULL,
            role     TEXT NOT NULL DEFAULT 'user',
            balance  REAL NOT NULL DEFAULT 0.0
        )
    """)

    users = [
        ('admin',   'SuperSecret@123', 'admin',  98500.00),
        ('alice',   'alice_pass',      'user',   1250.75),
        ('bob',     'bob_pass',        'user',   320.00),
        ('charlie', 'charlie_pass',    'user',   5480.50),
    ]
    cur.executemany(
        "INSERT INTO users (username, password, role, balance) VALUES (?,?,?,?)",
        users
    )

    # ── Comments table (for XSS demo) ────────────────────────────────────────
    cur.execute("DROP TABLE IF EXISTS comments")
    cur.execute("""
        CREATE TABLE comments (
            id        INTEGER PRIMARY KEY AUTOINCREMENT,
            author    TEXT NOT NULL,
            content   TEXT NOT NULL,
            timestamp TEXT NOT NULL DEFAULT (datetime('now','localtime'))
        )
    """)

    comments = [
        ('Alice',   'Great tutorial! Really helped me understand Flask.'),
        ('Bob',     'The dark theme looks amazing 🔥'),
        ('Charlie', 'Security is so important — thanks for the demo!'),
    ]
    cur.executemany(
        "INSERT INTO comments (author, content) VALUES (?,?)",
        comments
    )

    # ── Products table (for Parameter Tampering demo) ─────────────────────────
    cur.execute("DROP TABLE IF EXISTS products")
    cur.execute("""
        CREATE TABLE products (
            id    INTEGER PRIMARY KEY AUTOINCREMENT,
            name  TEXT NOT NULL,
            price REAL NOT NULL,
            stock INTEGER NOT NULL DEFAULT 10
        )
    """)

    products = [
        ('iPhone 16 Pro',     129999.00, 5),
        ('MacBook Air M3',    114999.00, 3),
        ('Sony WH-1000XM5',   29999.00, 8),
        ('Samsung 4K Monitor', 45999.00, 4),
    ]
    cur.executemany(
        "INSERT INTO products (name, price, stock) VALUES (?,?,?)",
        products
    )

    # ── Orders table (for Parameter Tampering demo) ────────────────────────────
    cur.execute("DROP TABLE IF EXISTS orders")
    cur.execute("""
        CREATE TABLE orders (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            product_name TEXT NOT NULL,
            real_price   REAL NOT NULL,
            paid_price   REAL NOT NULL,
            tampered     INTEGER NOT NULL DEFAULT 0,
            timestamp    TEXT NOT NULL DEFAULT (datetime('now','localtime'))
        )
    """)

    conn.commit()
    conn.close()
    print("[DB] Initialized and seeded demo.db")


if __name__ == '__main__':
    init_db()
