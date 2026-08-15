"""
generate_docs.py — Generates a clean PDF project report using ReportLab.
Run: python3 generate_docs.py
Output: SecureLab_Documentation.pdf
"""

from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib.units import cm
from reportlab.lib import colors
from reportlab.platypus import (
    SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle,
    HRFlowable, PageBreak, Preformatted
)
from reportlab.lib.enums import TA_CENTER, TA_LEFT, TA_JUSTIFY

# ── Colors ────────────────────────────────────────────────────────
BLUE    = colors.HexColor('#4a7fc1')
LBLUE   = colors.HexColor('#edf3fb')
RED     = colors.HexColor('#c95c5c')
LRED    = colors.HexColor('#fdf1f1')
GREEN   = colors.HexColor('#3e9e72')
LGREEN  = colors.HexColor('#edf7f2')
AMBER   = colors.HexColor('#b8820e')
GRAY    = colors.HexColor('#5a6b7e')
LGRAY   = colors.HexColor('#f4f6f9')
BORDER  = colors.HexColor('#e2e6ed')
BLACK   = colors.HexColor('#1e2732')
WHITE   = colors.white
CODE_BG = colors.HexColor('#f8f9fc')

W, H = A4

# ── Styles ────────────────────────────────────────────────────────
styles = getSampleStyleSheet()

def make_style(name, **kwargs):
    return ParagraphStyle(name, **kwargs)

TITLE = make_style('DocTitle',
    fontSize=22, fontName='Helvetica-Bold',
    textColor=BLACK, alignment=TA_CENTER, spaceAfter=6)

SUBTITLE = make_style('DocSubtitle',
    fontSize=11, fontName='Helvetica',
    textColor=GRAY, alignment=TA_CENTER, spaceAfter=4)

H1 = make_style('H1',
    fontSize=14, fontName='Helvetica-Bold',
    textColor=BLUE, spaceBefore=18, spaceAfter=6,
    borderPad=4)

H2 = make_style('H2',
    fontSize=11, fontName='Helvetica-Bold',
    textColor=BLACK, spaceBefore=10, spaceAfter=4)

H3 = make_style('H3',
    fontSize=10, fontName='Helvetica-Bold',
    textColor=GRAY, spaceBefore=8, spaceAfter=3)

BODY = make_style('Body',
    fontSize=10, fontName='Helvetica',
    textColor=BLACK, leading=15, spaceAfter=6,
    alignment=TA_JUSTIFY)

BULLET = make_style('Bullet',
    fontSize=10, fontName='Helvetica',
    textColor=BLACK, leading=14, spaceAfter=3,
    leftIndent=14, firstLineIndent=-10)

CODE = make_style('Code',
    fontSize=8.5, fontName='Courier',
    textColor=colors.HexColor('#1e2732'),
    backColor=CODE_BG, leading=13,
    leftIndent=8, rightIndent=8,
    borderPad=6, spaceBefore=4, spaceAfter=6)

CAPTION = make_style('Caption',
    fontSize=8.5, fontName='Helvetica',
    textColor=GRAY, alignment=TA_CENTER, spaceAfter=8)

def divider(color=BORDER):
    return HRFlowable(width='100%', thickness=1, color=color, spaceAfter=8, spaceBefore=4)

def section(title):
    return [Paragraph(title, H1), divider(BLUE)]

def subsection(title):
    return [Paragraph(title, H2)]

def body(text):
    return Paragraph(text, BODY)

def bullet(text):
    return Paragraph(f'• {text}', BULLET)

def code(text):
    return Preformatted(text, CODE)

def sp(n=8):
    return Spacer(1, n)

def table(data, col_widths, style_cmds=None):
    base = [
        ('BACKGROUND', (0,0), (-1,0), BLUE),
        ('TEXTCOLOR',  (0,0), (-1,0), WHITE),
        ('FONTNAME',   (0,0), (-1,0), 'Helvetica-Bold'),
        ('FONTSIZE',   (0,0), (-1,-1), 9),
        ('FONTNAME',   (0,1), (-1,-1), 'Helvetica'),
        ('ROWBACKGROUNDS', (0,1), (-1,-1), [WHITE, LGRAY]),
        ('GRID',       (0,0), (-1,-1), 0.5, BORDER),
        ('VALIGN',     (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 5),
        ('BOTTOMPADDING', (0,0), (-1,-1), 5),
        ('LEFTPADDING', (0,0), (-1,-1), 8),
    ]
    if style_cmds:
        base.extend(style_cmds)
    t = Table(data, colWidths=col_widths)
    t.setStyle(TableStyle(base))
    return t

# ── Build document ────────────────────────────────────────────────
def build():
    doc = SimpleDocTemplate(
        'SecureLab_Documentation.pdf',
        pagesize=A4,
        leftMargin=2.5*cm, rightMargin=2.5*cm,
        topMargin=2.5*cm, bottomMargin=2.5*cm,
        title='SecureLab — IS Lab CAT1 Project Report',
        author='IS Lab Student'
    )

    story = []

    # ═══════════════════════════════════════
    # COVER
    # ═══════════════════════════════════════
    story += [
        sp(40),
        Paragraph('SecureLab', TITLE),
        Paragraph('Web Security Attack Simulations', SUBTITLE),
        sp(6),
        divider(BLUE),
        sp(6),
        Paragraph('IS Lab CAT1 Project Report', SUBTITLE),
        Paragraph('SQL Injection · Cross-Site Scripting · Parameter Tampering', SUBTITLE),
        sp(30),
        table(
            [['Field', 'Details'],
             ['Course', 'Information Security Lab (IS Lab)'],
             ['Project Type', 'CAT1 — Web Application Security'],
             ['Framework', 'Python Flask + SQLite + HTML/CSS/JS'],
             ['Date', 'August 2026']],
            [5*cm, 10*cm]
        ),
        PageBreak()
    ]

    # ═══════════════════════════════════════
    # 1. ABSTRACT
    # ═══════════════════════════════════════
    story += section('1. Abstract')
    story.append(body(
        'This project demonstrates three critical web application security vulnerabilities '
        'through a fully interactive simulation platform called <b>SecureLab</b>. Built using '
        'Python Flask, SQLite, and HTML/CSS/JavaScript, the application allows users to execute '
        'live attacks — SQL Injection, Cross-Site Scripting (XSS), and Parameter Tampering — '
        'in a controlled environment. Each attack is paired with a secure implementation, '
        'illustrating the exact fix required. The project covers all evaluation aspects: '
        'simulation, rectification, design, viva, and documentation.'
    ))
    story.append(sp())

    # ═══════════════════════════════════════
    # 2. INTRODUCTION
    # ═══════════════════════════════════════
    story += section('2. Introduction')
    story.append(body(
        'Web applications form the backbone of modern digital infrastructure — from banking '
        'to e-commerce to social media. Poorly coded applications expose millions of users to '
        'attackers who exploit subtle but catastrophic flaws in how user input is handled.'
    ))
    story.append(body(
        'The <b>OWASP Top 10</b> (Open Web Application Security Project) is the industry-standard '
        'list of the most critical web security risks. Three of the most impactful attacks covered '
        'in this project are ranked in the top 4 of this list:'
    ))
    for item in [
        'SQL Injection — OWASP A03:2021 Injection',
        'Cross-Site Scripting (XSS) — OWASP A03:2021 Injection',
        'Parameter Tampering — OWASP A04:2021 Insecure Design',
    ]:
        story.append(bullet(item))
    story.append(sp())

    # ═══════════════════════════════════════
    # 3. TECH STACK
    # ═══════════════════════════════════════
    story += section('3. Technology Stack')
    story.append(table(
        [['Layer', 'Technology', 'Purpose'],
         ['Backend', 'Python + Flask 3.1', 'Route handling, business logic'],
         ['Database', 'SQLite (built-in)', 'Stores users, comments, products, orders'],
         ['Frontend', 'HTML5 + CSS3', 'Structure and minimal light-mode styling'],
         ['Scripting', 'Vanilla JavaScript', 'AJAX calls, dynamic UI updates'],
         ['Templating', 'Jinja2', 'Server-side HTML rendering'],
         ['Fonts', 'Inter + JetBrains Mono', 'UI typography and code blocks']],
        [3.5*cm, 4.5*cm, 7.5*cm]
    ))
    story.append(sp())
    story.append(body(
        '<b>Why Flask?</b> Flask is a lightweight Python web framework widely used for security '
        'demonstrations. Its minimal overhead makes raw SQL queries, unescaped output, and '
        'parameter handling visible without framework-level abstractions hiding the vulnerability.'
    ))
    story.append(sp())

    # ═══════════════════════════════════════
    # 4. ATTACK 1 — SQL INJECTION
    # ═══════════════════════════════════════
    story += [PageBreak()]
    story += section('4. Attack 1 — SQL Injection')

    story += subsection('4.1 Concept')
    story.append(body(
        'SQL Injection (SQLi) is a technique that exploits vulnerabilities in web applications '
        'by inserting malicious SQL statements into input fields. When user input is directly '
        'concatenated into a SQL query string, the attacker can alter the query\'s logic entirely — '
        'bypassing authentication, extracting data, or destroying the database.'
    ))

    story += subsection('4.2 Scenario')
    story.append(body(
        'A fake banking login page (FakeBank) is used. The database contains an admin user with '
        'a secret password. The attacker\'s goal is to log in as admin without knowing the password.'
    ))

    story += subsection('4.3 Vulnerable Code')
    story.append(code(
'''# VULNERABLE — raw string concatenation (DO NOT USE IN PRODUCTION)
username = request.form.get('username', '')
password = request.form.get('password', '')

query = f"SELECT * FROM users WHERE username='{username}' AND password='{password}'"
result = conn.execute(query).fetchone()'''
    ))

    story += subsection('4.4 Attack Payload & Execution')
    story.append(body('Attacker enters: <b>username = admin\'  --</b>  &nbsp; password = (anything)'))
    story.append(code(
'''-- Query the DB actually executes:
SELECT * FROM users WHERE username='admin' --' AND password='anything'
--  ↑ The -- comments out the entire password check → admin access granted!'''
    ))

    story += subsection('4.5 Secure Code — Parameterized Query')
    story.append(code(
'''# SECURE — parameterized query, values bound separately from query template
query = "SELECT * FROM users WHERE username = ? AND password = ?"
result = conn.execute(query, (username, password)).fetchone()
# The DB treats 'admin' -- as a plain string — never as SQL code'''
    ))

    story += subsection('4.6 Prevention Summary')
    for item in [
        'Always use parameterized queries or prepared statements',
        'Use ORMs (SQLAlchemy, Django ORM) which parameterize by default',
        'Apply least privilege to the database user account',
        'Validate and whitelist input where possible',
        'NEVER concatenate user input directly into SQL strings',
    ]:
        story.append(bullet(item))
    story.append(sp())

    # ═══════════════════════════════════════
    # 5. ATTACK 2 — XSS
    # ═══════════════════════════════════════
    story += [PageBreak()]
    story += section('5. Attack 2 — Cross-Site Scripting (XSS)')

    story += subsection('5.1 Concept')
    story.append(body(
        'XSS occurs when an attacker injects malicious JavaScript into web pages viewed by '
        'other users. The injected script runs in the victim\'s browser with the same trust '
        'level as the legitimate site — giving the attacker access to cookies, session tokens, '
        'and the ability to manipulate page content.'
    ))
    story.append(table(
        [['Type', 'Description'],
         ['Stored XSS (Demonstrated)', 'Script saved to DB, executes for every visitor'],
         ['Reflected XSS', 'Script in URL, executes when link is opened'],
         ['DOM-based XSS', 'Script manipulates DOM via client-side JS']],
        [5*cm, 10*cm]
    ))
    story.append(sp())

    story += subsection('5.2 Scenario')
    story.append(body(
        'A comment board is used. Users post comments that all visitors see. The attacker '
        'posts a comment containing JavaScript instead of text. Every subsequent visitor '
        'who loads the page has the script execute in their browser.'
    ))

    story += subsection('5.3 Vulnerable Code')
    story.append(code(
'''# Python — stores raw content without sanitisation
content = request.form.get('content', '')
conn.execute("INSERT INTO comments (author, content) VALUES (?, ?)", (author, content))

// JavaScript — renders as HTML (DANGEROUS)
commentDiv.innerHTML = comment.content;   // <script> tags EXECUTE here'''
    ))

    story += subsection('5.4 Attack Payloads')
    story.append(code(
'''<!-- Proof of concept -->
<script>alert('XSS! You have been hacked!')</script>

<!-- Cookie theft simulation -->
<img src=x onerror="alert('Cookie: ' + document.cookie)">

<!-- Page defacement -->
<b style='color:red;font-size:2rem'>Page Defaced!</b>'''
    ))

    story += subsection('5.5 Secure Code')
    story.append(code(
'''# Python — escape before storing
import html
content = html.escape(request.form.get('content', ''))
# <script> becomes &lt;script&gt; — displayed, never executed

// JavaScript — safe rendering
commentDiv.textContent = comment.content;   // treated as plain text only'''
    ))

    story += subsection('5.6 Prevention Summary')
    for item in [
        'Use html.escape() (Python) or equivalent on all user-supplied output',
        'Use textContent instead of innerHTML in JavaScript',
        'Implement Content Security Policy (CSP) headers',
        'Jinja2\'s {{ var }} auto-escapes — never use {{ var | safe }} with user data',
        'Validate and sanitise server-side, not just client-side',
    ]:
        story.append(bullet(item))
    story.append(sp())

    # ═══════════════════════════════════════
    # 6. ATTACK 3 — PARAMETER TAMPERING
    # ═══════════════════════════════════════
    story += [PageBreak()]
    story += section('6. Attack 3 — Parameter Tampering')

    story += subsection('6.1 Concept')
    story.append(body(
        'Parameter Tampering exploits the fact that web applications often pass business-critical '
        'values (prices, discounts, user IDs, roles) as hidden form fields, URL parameters, or '
        'cookies — trusting that the client will not modify them. An attacker uses browser '
        'DevTools or an intercepting proxy to alter these values before submission.'
    ))

    story += subsection('6.2 Scenario')
    story.append(body(
        'A fake e-commerce shop is used. Products have prices stored in the database. '
        'The hidden form field sends the price to the server when the user clicks Buy. '
        'The attacker modifies this field using DevTools to pay ₹1 for a ₹1,29,999 item.'
    ))

    story += subsection('6.3 Vulnerable Code')
    story.append(code(
'''# VULNERABLE — reads price from the client form (attacker controls this!)
product_id = request.form.get('product_id', type=int)
paid_price  = request.form.get('price', type=float)    # ← FROM CLIENT

conn.execute("INSERT INTO orders VALUES (?, ?, ?)", (product, real_price, paid_price))
# If attacker sets price=1, the order is recorded at ₹1 — no validation!'''
    ))

    story += subsection('6.4 Attack Steps')
    for i, step in enumerate([
        'Select iPhone 16 Pro (₹1,29,999) — page renders hidden field: value="129999"',
        'Open browser DevTools (F12) → Inspector → find the hidden price input',
        'Change value="129999" to value="1"',
        'Click Place Order — server receives paid_price=1',
        'Order log shows: iPhone 16 Pro bought for ₹1 — TAMPERED!',
    ], 1):
        story.append(bullet(f'Step {i}: {step}'))
    story.append(sp())

    story += subsection('6.5 Secure Code')
    story.append(code(
'''# SECURE — client-supplied price is completely IGNORED
product_id = request.form.get('product_id', type=int)

# Fetch the authoritative price from the database
row = conn.execute("SELECT price FROM products WHERE id=?", (product_id,)).fetchone()
server_price = row['price']    # DB is the single source of truth

conn.execute("INSERT INTO orders VALUES (?, ?, ?)", (product, server_price, server_price))
# Tamper attempt detected and logged; real price always charged'''
    ))

    story += subsection('6.6 Prevention Summary')
    for item in [
        'Never trust client-supplied prices, discounts, or quantities',
        'Always fetch authoritative values from the server-side database',
        'Validate all parameters: type, range, whitelist of allowed values',
        'Sign or HMAC-encrypt sensitive parameters if they must travel client-side',
        'Use server-side sessions for sensitive state instead of client-side storage',
    ]:
        story.append(bullet(item))
    story.append(sp())

    # ═══════════════════════════════════════
    # 7. RESULTS
    # ═══════════════════════════════════════
    story += [PageBreak()]
    story += section('7. Results & Observations')

    story.append(table(
        [['Attack', 'Vulnerable Mode Result', 'Secure Mode Result'],
         ['SQL Injection',
          "admin' -- bypasses login. Admin account with ₹98,500 balance fully exposed.",
          'Login fails. Parameterized query rejects the payload. No bypass possible.'],
         ['XSS',
          '<script>alert()</script> executes in browser. Cookies accessible via onerror.',
          'Script rendered as plain text. html.escape() converts < to &lt;.'],
         ['Parameter Tampering',
          'iPhone 16 Pro (₹1,29,999) purchased for ₹1. Server trusts client price.',
          'Server fetches DB price. Client value ignored. Real price (₹1,29,999) charged.']],
        [3.5*cm, 6.5*cm, 5.5*cm]
    ))
    story.append(sp())

    # ═══════════════════════════════════════
    # 8. CONCLUSION
    # ═══════════════════════════════════════
    story += section('8. Conclusion')
    story.append(body(
        'Web security vulnerabilities remain among the most prevalent and costly threats in '
        'software today. Through this project, we demonstrated that each attack is eliminated '
        'by a focused, minimal code change:'
    ))
    for item in [
        'SQL Injection → Parameterized queries (one line change)',
        'XSS → html.escape() + textContent instead of innerHTML',
        'Parameter Tampering → Server-side price validation from the database',
    ]:
        story.append(bullet(item))
    story.append(sp(6))
    story.append(body(
        'These three fixes together embody one of the most important principles of secure '
        'web development: <b>never trust the client</b>. Validate, sanitise, and verify '
        'everything on the server.'
    ))
    story.append(sp())

    # ═══════════════════════════════════════
    # 9. REFERENCES
    # ═══════════════════════════════════════
    story += section('9. References')
    refs = [
        'OWASP Top 10 (2021) — https://owasp.org/www-project-top-ten/',
        'OWASP SQL Injection — https://owasp.org/www-community/attacks/SQL_Injection',
        'OWASP Cross-Site Scripting — https://owasp.org/www-community/attacks/xss/',
        'Flask Documentation — https://flask.palletsprojects.com/',
        'Python html.escape() — https://docs.python.org/3/library/html.html',
        'Python sqlite3 — https://docs.python.org/3/library/sqlite3.html',
        'Heartland Payment Breach (2008) — Krebs on Security',
        'Samy Worm Analysis (2005) — https://samy.pl/myspace/tech.html',
    ]
    for i, r in enumerate(refs, 1):
        story.append(bullet(f'[{i}] {r}'))

    doc.build(story)
    print('[PDF] SecureLab_Documentation.pdf generated successfully!')

if __name__ == '__main__':
    build()
