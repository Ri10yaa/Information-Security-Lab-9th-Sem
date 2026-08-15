/* ─── main.js — Shared utilities for Security Attack Demo ─── */

// ── Toast notifications ───────────────────────────────────────────────────────
function showToast(message, type = 'success') {
  const container = document.getElementById('toast-container') ||
    (() => {
      const c = document.createElement('div');
      c.id = 'toast-container';
      c.className = 'toast-container';
      document.body.appendChild(c);
      return c;
    })();

  const t = document.createElement('div');
  t.className = `toast toast-${type}`;
  t.textContent = message;
  container.appendChild(t);
  setTimeout(() => {
    t.style.animation = 'slideOut 0.3s ease forwards';
    setTimeout(() => t.remove(), 300);
  }, 3200);
}

// ── Terminal helper ───────────────────────────────────────────────────────────
function termLine(terminal, prompt, text, cls = '') {
  const line = document.createElement('div');
  line.className = 'line';
  line.innerHTML = `<span class="prompt">${prompt}</span><span class="text ${cls}">${text}</span>`;
  terminal.appendChild(line);
  terminal.scrollTop = terminal.scrollHeight;
}

function clearTerminal(id) {
  const t = document.getElementById(id);
  if (t) t.innerHTML = '';
}

// ── Fetch wrapper with loading state ─────────────────────────────────────────
async function postData(url, formData) {
  const res = await fetch(url, { method: 'POST', body: formData });
  if (!res.ok) throw new Error(`HTTP ${res.status}`);
  return res.json();
}

// ── Payload chip click → fill input ──────────────────────────────────────────
document.addEventListener('DOMContentLoaded', () => {
  document.querySelectorAll('.chip[data-target][data-value]').forEach(chip => {
    chip.addEventListener('click', () => {
      const el = document.getElementById(chip.dataset.target);
      if (el) {
        el.value = chip.dataset.value;
        el.focus();
        el.dispatchEvent(new Event('input'));
      }
    });
  });
});

// ── Active nav link ───────────────────────────────────────────────────────────
document.addEventListener('DOMContentLoaded', () => {
  const path = window.location.pathname;
  document.querySelectorAll('.nav-links a').forEach(a => {
    if (a.getAttribute('href') === path) a.classList.add('active');
  });
});
