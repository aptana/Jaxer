cfg = {
        'jam_install':          '/opt/AptanaJaxer',
        'unzip_location':       '/opt',
        'start_servers':        '/opt/AptanaJaxer/scripts/start.sh',
        'stop_servers':         '/opt/AptanaJaxer/scripts/stop.sh',
        'httpd_pid':            '/opt/AptanaJaxer/Apache22/logs/httpd.pid',
        'results_file':         '/opt/AptanaJaxer/jaxer/aptana/diagnostics/results.html',
        'jam_package':          '/home/cltbld/dev/trunk/products/server/distro/Jaxer_package_withApache.zip',
        'jam_mount':            '',

        'apache-name':          'httpd',
        'apxs-name':		        '/opt/AptanaJaxer/Apache22/bin/apxs',
        'jaxermanager-name':    'jaxermanager',
        'browser':              'firefox',
        'browser-name':         'firefox',
        'kill':                 'killall',

        'host':                 'http://localhost:8081',
        'page':                 'aptana/diagnostics/testRunner.html',
        'query':                'runAll=true&writeTo=results.html',

        'makeDocs':             True,
        'frameworkRoot':        '/home/cltbld/dev/trunk/products/server/src/mozilla/ff-release/dist/bin/framework',
        'docgenRoot':           '/home/cltbld/dev/ide_crossover/docgen/libs/',
        'docRoot':              '/home/cltbld/dev/trunk/products/server/jam/jaxer/aptana/doc/api/',

        'skipBuilds':           False
}

