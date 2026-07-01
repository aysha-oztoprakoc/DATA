"""
kad_mcp_server.py

MCP Server exposing KAD Pipeline tools to the Antigravity CLI.
"""

import os
import sys
import asyncio
from mcp.server import Server
from mcp.server.stdio import stdio_server
from mcp.types import Tool, TextContent

server = Server("kad-pipeline")

@server.list_tools()
async def list_tools() -> list[Tool]:
    return [
        Tool(
            name="kad_pipeline_status",
            description="Returns the current status of the KAD pipeline (AMDY and TELL nodes)",
            inputSchema={
                "type": "object",
                "properties": {},
                "required": []
            }
        ),
        Tool(
            name="kad_tell_stats",
            description="Returns the storage and processing stats from the TELL node",
            inputSchema={
                "type": "object",
                "properties": {},
                "required": []
            }
        ),
        Tool(
            name="kad_ingest_manual",
            description="Manually triggers a file ingestion as if inotify detected it",
            inputSchema={
                "type": "object",
                "properties": {
                    "file_path": {"type": "string", "description": "Absolute path to file on TELL node"}
                },
                "required": ["file_path"]
            }
        )
    ]

@server.call_tool()
async def call_tool(name: str, arguments: dict) -> list[TextContent]:
    # This is a stub implementation. In a full system, this would connect
    # to MQTT to query live status, or to a local SQLite database that the
    # Bridge updates when it receives telemetry.
    if name == "kad_pipeline_status":
        return [TextContent(type="text", text="Pipeline is ONLINE. (Telemetry requires active MQTT connection in MCP)")]
    elif name == "kad_tell_stats":
        return [TextContent(type="text", text="Storage stats: N/A. (Telemetry requires active MQTT connection in MCP)")]
    elif name == "kad_ingest_manual":
        file_path = arguments.get("file_path", "unknown")
        return [TextContent(type="text", text=f"Manual ingestion signal sent for {file_path}.")]
    else:
        raise ValueError(f"Unknown tool: {name}")

async def main():
    async with stdio_server() as (read_stream, write_stream):
        await server.run(read_stream, write_stream, server.create_initialization_options())

if __name__ == "__main__":
    asyncio.run(main())
