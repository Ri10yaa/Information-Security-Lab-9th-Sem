"""
app.py — Flask Security Attack Simulation
Demonstrates: SQL Injection, XSS, Parameter Tampering
Each attack has a vulnerable route and a secure (rectified) route.
"""

import html
import sqlite3
from datetime import datetime
from flask import (Flask, render_template, request,
                   redirect, url_for, session, jsonify)
from database import init_db, get_db, DB_PATH

app = Flask(__name__)
app.secret_key = 'dev-secret-key-not-for-production'

# ── Initialise DB on startup ──────────────────────────────────────────────────
with app.app_context():
    init_db()


# ════════════════════════════════════════════════════════════════════════════════
#  HOME
# ════════════════════════════════════════════════════════════════════════════════
@app.route('/')
def index():
    return render_template('index.html')


# ════════════════════════════════════════════════════════════════════════════════
#  ATTACK 1 — SQL INJECTION
# ════════════════════════════════════════════════════════════════════════════════
@app.route('/sql-injection')
def sql_injection():
    return render_template('sql_injection.html')


@app.route('/sql-injection/vulnerable', methods=['POST'])
def sql_injection_vulnerable():
    """
    ⚠️  DELIBERATELY VULNERABLE — DO NOT USE IN PRODUCTION
    Concatenates user input directly into the SQL query string.
    Payload: admin' --   OR   ' OR '1'='1
    """
    username = request.form.get('username', '')
    password = request.form.get('password', '')

    # VULNERABLE: raw string concatenation
    query = f"SELECT * FROM users WHERE username='{username}' AND password='{password}'"

    conn = get_db()
    try:
        result = conn.execute(query).fetchone()
        if result:
            response = {
                'status': 'success',
                'message': f"✅ Logged in as: {result['username']} (Role: {result['role']}, Balance: ₹{result['balance']:,.2f})",
                'query': query,
                'attack_worked': True,
                'user': dict(result)
            }
        else:
            response = {
                'status': 'failure',
                'message': '❌ Invalid credentials.',
                'query': query,
                'attack_worked': False
            }
    except sqlite3.OperationalError as e:
        response = {
            'status': 'error',
            'message': f'SQL Error: {str(e)}',
            'query': query,
            'attack_worked': False
        }
    finally:
        conn.close()

    return jsonify(response)


@app.route('/sql-injection/secure', methods=['POST'])
def sql_injection_secure():
    """
    ✅ SECURE — Uses parameterised queries (prepared statements).
    The DB driver treats user input as data, never as SQL code.
    """
    username = request.form.get('username', '')
    password = request.form.get('password', '')

    # SECURE: parameterised query — input is bound as a value, not code
    query = "SELECT * FROM users WHERE username = ? AND password = ?"

    conn = get_db()
    result = conn.execute(query, (username, password)).fetchone()
    conn.close()

    if result:
        response = {
            'status': 'success',
            'message': f"✅ Logged in as: {result['username']} (Role: {result['role']})",
            'query': f"{query}  →  params: ('{username}', '****')",
            'attack_worked': False
        }
    else:
        response = {
            'status': 'failure',
            'message': '❌ Invalid credentials. SQL injection blocked.',
            'query': f"{query}  →  params: ('{username}', '****')",
            'attack_worked': False
        }

    return jsonify(response)


# ════════════════════════════════════════════════════════════════════════════════
#  ATTACK 2 — CROSS-SITE SCRIPTING (XSS)
# ════════════════════════════════════════════════════════════════════════════════
@app.route('/xss')
def xss():
    conn = get_db()
    comments = conn.execute(
        "SELECT * FROM comments ORDER BY id DESC"
    ).fetchall()
    conn.close()
    return render_template('xss.html', comments=comments)


@app.route('/xss/vulnerable', methods=['POST'])
def xss_vulnerable():
    """
    ⚠️  DELIBERATELY VULNERABLE
    Stores raw user input and renders it unescaped — allows script injection.
    Payload: <script>alert('XSS!')</script>
             <img src=x onerror="alert('Cookie: '+document.cookie)">
    """
    author  = request.form.get('author', 'Anonymous')
    content = request.form.get('content', '')

    # VULNERABLE: stores and returns raw HTML/JS without sanitisation
    conn = get_db()
    conn.execute(
        "INSERT INTO comments (author, content) VALUES (?, ?)",
        (author, content)   # stored as-is
    )
    conn.commit()
    comments = conn.execute(
        "SELECT * FROM comments ORDER BY id DESC LIMIT 10"
    ).fetchall()
    conn.close()

    comments_list = [dict(c) for c in comments]
    return jsonify({
        'status': 'success',
        'comments': comments_list,
        'escaped': False,   # frontend renders raw HTML
        'message': '⚠️ Comment stored WITHOUT sanitisation — script will execute!'
    })


@app.route('/xss/secure', methods=['POST'])
def xss_secure():
    """
    ✅ SECURE — HTML-escapes all user input before storage and display.
    <script> tags become &lt;script&gt; and are shown as plain text.
    """
    author  = html.escape(request.form.get('author', 'Anonymous'))
    content = html.escape(request.form.get('content', ''))

    conn = get_db()
    conn.execute(
        "INSERT INTO comments (author, content) VALUES (?, ?)",
        (author, content)
    )
    conn.commit()
    comments = conn.execute(
        "SELECT * FROM comments ORDER BY id DESC LIMIT 10"
    ).fetchall()
    conn.close()

    comments_list = [dict(c) for c in comments]
    return jsonify({
        'status': 'success',
        'comments': comments_list,
        'escaped': True,    # frontend renders as plain text
        'message': '✅ Comment sanitised — HTML/JS tags are escaped and harmless.'
    })


@app.route('/xss/reset', methods=['POST'])
def xss_reset():
    """Reset comments table to default seed data."""
    conn = get_db()
    conn.execute("DELETE FROM comments")
    conn.executemany(
        "INSERT INTO comments (author, content) VALUES (?,?)",
        [
            ('Alice',   'Great tutorial! Really helped me understand Flask.'),
            ('Bob',     'The dark theme looks amazing 🔥'),
            ('Charlie', 'Security is so important — thanks for the demo!'),
        ]
    )
    conn.commit()
    conn.close()
    return jsonify({'status': 'reset'})


# ════════════════════════════════════════════════════════════════════════════════
#  ATTACK 3 — PARAMETER TAMPERING
# ════════════════════════════════════════════════════════════════════════════════
@app.route('/parameter-tampering')
def parameter_tampering():
    conn = get_db()
    products = conn.execute("SELECT * FROM products").fetchall()
    orders   = conn.execute(
        "SELECT * FROM orders ORDER BY id DESC LIMIT 5"
    ).fetchall()
    conn.close()
    return render_template('param_tamper.html',
                           products=products, orders=orders)


@app.route('/parameter-tampering/vulnerable', methods=['POST'])
def param_tamper_vulnerable():
    """
    ⚠️  DELIBERATELY VULNERABLE
    Trusts the price sent from the client (hidden form field).
    Attacker changes price=129999 → price=1 in DevTools before submitting.
    """
    product_id   = request.form.get('product_id', type=int)
    product_name = request.form.get('product_name', '')
    paid_price   = request.form.get('price', type=float, default=0.0)  # CLIENT-SUPPLIED ← vulnerable

    conn = get_db()
    real_price = conn.execute(
        "SELECT price FROM products WHERE id=?", (product_id,)
    ).fetchone()['price']

    tampered = abs(paid_price - real_price) > 0.01

    conn.execute(
        "INSERT INTO orders (product_name, real_price, paid_price, tampered) VALUES (?,?,?,?)",
        (product_name, real_price, paid_price, int(tampered))
    )
    conn.commit()
    conn.close()

    return jsonify({
        'status': 'success',
        'product': product_name,
        'real_price': real_price,
        'paid_price': paid_price,
        'tampered': tampered,
        'message': (
            f"⚠️ Order placed at ₹{paid_price:,.2f}! Real price was ₹{real_price:,.2f}. "
            f"{'🚨 TAMPERED — You saved ₹' + f'{real_price-paid_price:,.2f}!' if tampered else 'Price was correct.'}"
        )
    })


@app.route('/parameter-tampering/secure', methods=['POST'])
def param_tamper_secure():
    """
    ✅ SECURE — Ignores client-supplied price entirely.
    Fetches the authoritative price from the server-side DB.
    """
    product_id   = request.form.get('product_id', type=int)
    product_name = request.form.get('product_name', '')
    client_price = request.form.get('price', type=float, default=0.0)

    conn = get_db()
    row = conn.execute(
        "SELECT price FROM products WHERE id=?", (product_id,)
    ).fetchone()
    server_price = row['price']  # SERVER authoritative price — ignores client value

    conn.execute(
        "INSERT INTO orders (product_name, real_price, paid_price, tampered) VALUES (?,?,?,?)",
        (product_name, server_price, server_price, 0)
    )
    conn.commit()
    conn.close()

    tampered_attempt = abs(client_price - server_price) > 0.01

    return jsonify({
        'status': 'success',
        'product': product_name,
        'server_price': server_price,
        'client_price': client_price,
        'tampered_attempt': tampered_attempt,
        'message': (
            f"✅ Order secured at real price ₹{server_price:,.2f}. "
            + (f"Tampering attempt detected! Client sent ₹{client_price:,.2f} but was ignored." if tampered_attempt else "")
        )
    })


@app.route('/parameter-tampering/orders')
def get_orders():
    conn = get_db()
    orders = conn.execute(
        "SELECT * FROM orders ORDER BY id DESC LIMIT 8"
    ).fetchall()
    conn.close()
    return jsonify([dict(o) for o in orders])


# ════════════════════════════════════════════════════════════════════════════════
if __name__ == '__main__':
    app.run(debug=True, port=5000)
