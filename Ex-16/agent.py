# /// script
# requires-python = ">=3.11"
# dependencies = [
#   "rich>=14.0.0",
#   "strands-agents[openai]>=1.0.0",
# ]
# ///

"""A small Strands chat agent with local input guardrails.

Run with: uv run agent.py
The model uses the MegaHub OpenAI-compatible endpoint and a GLM model.
"""

from __future__ import annotations

import json
import os
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from rich.console import Console  # type: ignore[import-not-found]
from rich.panel import Panel  # type: ignore[import-not-found]
from rich.prompt import Prompt  # type: ignore[import-not-found]
from rich.text import Text  # type: ignore[import-not-found]
from strands import Agent  # type: ignore[import-not-found]
from strands.models.openai import OpenAIModel  # type: ignore[import-not-found]

console = Console()

REFUSAL = "I can't process that request because it appears to be a prompt-injection attempt."
MEGAHUB_CONFIG = Path.home() / ".pi" / "agent" / "models.json"
MEGAHUB_MODEL = os.getenv("MEGAHUB_MODEL", "glm-5.1")

# These checks run locally, before any content is added to the Strands conversation.
INJECTION_PATTERNS = (
    re.compile(r"\b(ignore|disregard|forget|override|bypass)\b.{0,60}\b(previous|prior|above|system|developer|safety|original)\b.{0,30}\b(instruction|prompt|rule|message|policy|guardrail)s?\b", re.I),
    re.compile(r"\b(reveal|show|print|repeat|leak|expose|return)\b.{0,50}\b(system|developer|hidden|initial|secret)\b.{0,20}\b(prompt|instruction|message|rule)s?\b", re.I),
    re.compile(r"\b(system|developer)\s*(prompt|message)\s*[:=]", re.I),
    re.compile(r"\b(do not|don't)\s+(follow|obey)\b.{0,30}\b(instruction|rule|policy)s?\b", re.I),
    re.compile(r"\b(jailbreak|prompt injection|DAN mode|developer mode)\b", re.I),
    re.compile(r"\b(act|behave|respond)\s+as\s+(if\s+you\s+are\s+)?(unrestricted|unfiltered|DAN)\b", re.I),
)


@dataclass(frozen=True)
class PiiPattern:
    label: str
    pattern: re.Pattern[str]


PII_PATTERNS = (
    PiiPattern("EMAIL", re.compile(r"(?<![\w.+-])[\w.+-]+@[\w-]+(?:\.[\w-]+)+", re.I)),
    PiiPattern("SSN", re.compile(r"(?<!\d)\d{3}-\d{2}-\d{4}(?!\d)")),
    PiiPattern(
        "PHONE",
        re.compile(r"(?<!\w)(?:\+?\d{1,3}[ .-]?)?(?:\(?\d{3}\)?[ .-]?)\d{3}[ .-]\d{4}(?!\w)"),
    ),
    PiiPattern(
        "CREDIT_CARD",
        re.compile(r"(?<!\d)(?:\d[ -]?){13,19}(?!\d)"),
    ),
    PiiPattern(
        "IP_ADDRESS",
        re.compile(r"(?<!\d)(?:(?:25[0-5]|2[0-4]\d|1?\d?\d)\.){3}(?:25[0-5]|2[0-4]\d|1?\d?\d)(?!\d)"),
    ),
    PiiPattern(
        "DATE_OF_BIRTH",
        re.compile(r"\b(?:dob|date of birth|born on)\s*(?::|is)?\s*\d{1,2}[/-]\d{1,2}[/-]\d{2,4}\b", re.I),
    ),
    PiiPattern(
        "NAME",
        re.compile(r"\b(?:my name is|i am|i'm|this is)\s+([A-Z][a-z]+(?:[ -][A-Z][a-z]+){0,2})\b"),
    ),
)


def is_prompt_injection(text: str) -> bool:
    """Return True for common direct prompt-injection instructions."""
    normalized = " ".join(text.split())
    return any(pattern.search(normalized) for pattern in INJECTION_PATTERNS)


def _luhn_valid(candidate: str) -> bool:
    digits = [ord(char) - ord("0") for char in candidate if "0" <= char <= "9"]
    if not 13 <= len(digits) <= 19 or len(set(digits)) == 1:
        return False
    total = 0
    parity = len(digits) % 2
    for index, digit in enumerate(digits):
        if index % 2 == parity:
            digit *= 2
            if digit > 9:
                digit -= 9
        total += digit
    return total % 10 == 0


def mask_pii(text: str) -> tuple[str, set[str]]:
    """Mask locally recognizable PII and return the detected categories."""
    masked = text
    detected: set[str] = set()

    for item in PII_PATTERNS:
        def replace(match: re.Match[str], pii: PiiPattern = item) -> str:
            if pii.label == "CREDIT_CARD" and not _luhn_valid(match.group(0)):
                return match.group(0)
            detected.add(pii.label)
            if pii.label == "NAME":
                prefix = match.group(0)[: match.start(1) - match.start(0)]
                return f"{prefix}[MASKED_NAME]"
            return f"[MASKED_{pii.label}]"

        masked = item.pattern.sub(replace, masked)

    return masked, detected


def load_megahub_credentials() -> tuple[str, str]:
    """Load MegaHub credentials from environment or the local Pi provider config."""
    api_key = os.getenv("MEGAHUB_API_KEY")
    base_url = os.getenv("MEGAHUB_BASE_URL")

    if api_key and base_url:
        return api_key, base_url

    try:
        config: dict[str, Any] = json.loads(MEGAHUB_CONFIG.read_text(encoding="utf-8"))
        provider = config["providers"]["mega-hub"]
        api_key = api_key or provider["apiKey"]
        base_url = base_url or provider["baseUrl"]
    except (OSError, ValueError, KeyError, TypeError) as error:
        raise RuntimeError(
            "MegaHub credentials were not found. Set MEGAHUB_API_KEY and "
            "MEGAHUB_BASE_URL, or configure the mega-hub provider in Pi."
        ) from error

    if not isinstance(api_key, str) or not api_key or not isinstance(base_url, str) or not base_url:
        raise RuntimeError("MegaHub API key or base URL is empty.")
    return api_key, base_url


def build_agent() -> Agent:
    api_key, base_url = load_megahub_credentials()
    model = OpenAIModel(
        client_args={"api_key": api_key, "base_url": base_url},
        model_id=MEGAHUB_MODEL,
        params={"max_tokens": 1200, "temperature": 0.3},
    )
    return Agent(
        model=model,
        system_prompt=(
            "You are a concise, friendly general-purpose assistant. The application replaces "
            "personal data with tokens such as [MASKED_EMAIL]. Never ask the user to reveal "
            "masked personal data, and do not try to infer it."
        ),
        callback_handler=None,
    )


def print_banner() -> None:
    console.print()
    console.print(
        Panel.fit(
            "[bold cyan]Strands Safe Chat[/bold cyan]\n"
            f"[dim]MegaHub / {MEGAHUB_MODEL} • PII masking • Injection refusal[/dim]",
            border_style="cyan",
            padding=(1, 4),
        )
    )
    console.print("[dim]Type [bold]exit[/bold] or [bold]quit[/bold] to end the session.[/dim]\n")


def main() -> None:
    print_banner()
    try:
        agent = build_agent()
    except RuntimeError as error:
        console.print(Panel(str(error), title="[bold red]Configuration error[/bold red]", border_style="red"))
        raise SystemExit(1) from error

    while True:
        try:
            user_input = Prompt.ask("[bold green]You[/bold green]").strip()
        except (EOFError, KeyboardInterrupt):
            console.print("\n[dim]Goodbye.[/dim]\n")
            break

        if not user_input:
            console.print("[yellow]Please enter a message.[/yellow]\n")
            continue
        if user_input.lower() in {"exit", "quit"}:
            console.print("\n[dim]Goodbye.[/dim]\n")
            break

        if is_prompt_injection(user_input):
            console.print()
            console.print(Panel(REFUSAL, title="[bold red]Request refused[/bold red]", border_style="red"))
            console.print()
            continue

        masked_input, detected = mask_pii(user_input)
        labels = ", ".join(sorted(detected)) if detected else "none"
        preview = Text()
        preview.append("Sent to model", style="bold yellow")
        preview.append(f" (PII detected: {labels})\n", style="dim")
        preview.append(masked_input, style="yellow")
        console.print(preview)
        console.print()

        try:
            response = agent(masked_input)
        except Exception as error:
            console.print(Panel(str(error), title="[bold red]Model error[/bold red]", border_style="red"))
            console.print()
            continue

        console.print("[bold blue]Assistant[/bold blue]")
        console.print(Panel(str(response), border_style="blue", padding=(1, 2)))
        console.print()


if __name__ == "__main__":
    main()
