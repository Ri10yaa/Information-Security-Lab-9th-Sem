# /// script
# requires-python = ">=3.11"
# dependencies = [
#   "fastapi>=0.116.0",
#   "rich>=14.0.0",
#   "strands-agents[openai]>=1.0.0",
#   "uvicorn>=0.35.0",
# ]
# ///

"""Minimal HTTP wrapper for testing the safe chat flow with Burp Suite.

Run with: uv run server.py
Send POST requests to: http://127.0.0.1:8000/chat
"""

from __future__ import annotations

from contextlib import asynccontextmanager
from typing import AsyncIterator

import uvicorn  # type: ignore[import-not-found]
from fastapi import FastAPI, HTTPException, Request  # type: ignore[import-not-found]
from pydantic import BaseModel, Field  # type: ignore[import-not-found]
from starlette.concurrency import run_in_threadpool  # type: ignore[import-not-found]
from strands import Agent  # type: ignore[import-not-found]

from agent import REFUSAL, build_agent, is_prompt_injection, mask_pii


class ChatRequest(BaseModel):
    message: str = Field(min_length=1, max_length=10_000)


class ChatResponse(BaseModel):
    status: str
    masked_input: str
    pii_detected: list[str]
    response: str


@asynccontextmanager
async def lifespan(app: FastAPI) -> AsyncIterator[None]:
    app.state.agent = build_agent()
    yield


app = FastAPI(
    title="Burp Safe Chat Test Server",
    docs_url=None,
    redoc_url=None,
    openapi_url=None,
    lifespan=lifespan,
)


@app.post("/chat", response_model=ChatResponse)
async def chat(payload: ChatRequest, request: Request) -> ChatResponse:
    """Reject prompt injection, mask PII, then send sanitized input to GLM."""
    message = payload.message.strip()
    if not message:
        raise HTTPException(status_code=422, detail="message must not be blank")

    if is_prompt_injection(message):
        raise HTTPException(status_code=403, detail=REFUSAL)

    masked_input, detected = mask_pii(message)
    strands_agent: Agent = request.app.state.agent

    try:
        result = await run_in_threadpool(strands_agent, masked_input)
    except Exception as error:
        raise HTTPException(status_code=502, detail="The model request failed") from error

    return ChatResponse(
        status="ok",
        masked_input=masked_input,
        pii_detected=sorted(detected),
        response=str(result),
    )


if __name__ == "__main__":
    uvicorn.run(app, host="127.0.0.1", port=8000, access_log=True)
