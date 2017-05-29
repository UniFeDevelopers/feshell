module.exports = function(grunt) {
    grunt.initConfig({
        exec: {
            make: {
                cmd: 'make && make test && make clean',
                stdout: true,
                stderr: true
            }
        },
        watch: {
            Makefile: {
                files: 'Makefile',
                options: {
                    reload: true
                },
                tasks: ['exec:make']
            },
            sources: {
                files: ['./src/*.c', './src/ls/*.c', './src/parse_lib/*.c'],
                options: {
                    reload: true
                },
                tasks: ['exec:make']
            }
        }
    });

    grunt.loadNpmTasks('grunt-exec');
    grunt.loadNpmTasks('grunt-contrib-watch');

    grunt.registerTask('make', 'make');
    grunt.registerTask('default', ['watch']);
};
