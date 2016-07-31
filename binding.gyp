{
  "targets": [
    {
      "target_name": "ctp",
      "sources": [
        "ctp.cc",
        "wrap_trader.cpp",
        "uv_trader.cpp"
      ],
      "libraries": [
        "..\\tradeapi\\thosttraderapi.lib"
      ],
      "include_dirs": [".\\tradeapi\\"]
    }
  ]
}
