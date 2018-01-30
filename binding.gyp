{
  "targets": [
    {
      "target_name": "winfileinfo",
      "sources": [ "src/fileinfo.cpp" ]
    },
    {
      "target_name": "copy_module",
      "type":"none",
      "dependencies" : [ "winfileinfo" ],
      "copies":
      [
        {
          'destination': '<(module_root_dir)',
          'files': ['<(module_root_dir)/build/Release/winfileinfo.node']
        }
      ]
    }
  ]
}