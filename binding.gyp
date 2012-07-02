{
    'targets': [
        {
            'target_name': 'girepository',
            'type': '<(library)',
            'sources': [
                'src/init.cc',
                'src/util.cc',
                'src/namespace_loader.cc',
                'src/arguments.cc',
                'src/function.cc',
                'src/values.cc',
                'src/types/object.cc',
                'src/types/struct.cc',
                'src/types/function.cc'
            ],
            'conditions': [
                ['OS=="linux"',
                    {
                        'libraries': [
                            '<!@(pkg-config --libs glib-2.0 gobject-introspection-1.0)'
                        ],
                        'cflags': [
                            '<!@(pkg-config --cflags glib-2.0 gobject-introspection-1.0)',
                            '-D_FILE_OFFSET_BITS=64',
                            '-D_LARGEFILE_SOURCE'
                        ],
                        'ldflags': [
                            '<!@(pkg-config --libs glib-2.0 gobject-introspection-1.0)'
                        ]
                    }
                ]
            ]
        }
    ]
}
