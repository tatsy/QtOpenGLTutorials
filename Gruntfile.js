'use strict';

var fs   = require('fs');
var path = require('path');

module.exports = function(grunt) {
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    watch: {
      options: {
        livereload: true
      },
      jade: {
        tasks: ["jade:compile"],
        files: ["**/*.jade", "!layouts/*.jade", "!node_modules/**/*.jade"]
      },
      copy: {
        tasks: ["copy"],
        files: ["js/*.js", "css/*.css", "!node_modules/**/*.js", "!node_modules/**/*.css"]
      }
    },
    jade: {
      compile: {
        options: {
          pretty: true,
          data: function(dest, src) {
            var rootdir = '../sources/'
            var files = fs.readdirSync(rootdir).filter(function(file) {
              return fs.statSync(rootdir + file).isDirectory();
            });

            var data = {};
            data.repodir = "https://github.com/tatsy/QtOpenGLTutorials/tree/master";
            data.reporaw = "https://raw.githubusercontent.com/tatsy/QtOpenGLTutorials/master";
            data.list = files;
            return data;
          }
        },
        files: {
          "index.html": "src/index.jade"
        }
      }
    },
    copy: {
      main: {
        files: [
          {
            expand: true,
            cwd: 'bower_components/',
            src: ['**/*.min.js'],
            dest: 'js',
            flatten: true
          },
          {
            expand: true,
            cwd: 'bower_components/',
            src: ['**/*.min.css'],
            dest: 'css',
            flatten: true
          },
          {
            expand: true,
            cwd: 'bower_components/',
            src: ['**/fonts/*'],
            dest: 'fonts',
            flatten: true,
            filter: 'isFile'
          }
        ]
      }
    }
  });

  grunt.loadNpmTasks('grunt-contrib-watch');
  grunt.loadNpmTasks('grunt-contrib-copy');
  grunt.loadNpmTasks('grunt-contrib-jade');
  grunt.registerTask('default', ['jade:compile', 'copy']);
}
