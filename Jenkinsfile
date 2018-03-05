import groovy.transform.InheritConstructors

stage('Linux') {
    def linuxBuild = new LinuxBuild(this, 'Linux')
        .withDebugBuild()
        .withCodeCoverage()
        .withTests()
        .withAsan()

    runBuild(linuxBuild, 'ignite-linux', 'ignite-linux')
}

stage('OSX') {
    def osxBuild = new LinuxBuild(this, 'OSX')
        .withDebugBuild()
        .withTests()

    withEnv (['PATH+MAVEN=/usr/local/bin']) {
        runBuild(osxBuild, 'hawktracer-osx', 'hawktracer-osx')
    }
}

stage('Raspberry PI') {
    def rpiBuild = new RaspberryBuild(this, 'Raspberry PI')
        .withToolchain('../rpi-toolchain.cmake')
        .withInitialCacheFile('../TryRunResults.cmake')
        .withBenchmarks()

    runBuild(rpiBuild, 'ignite-build', 'hawktracer-rpi')
}

def runBuild(HTBuild htBuild, String buildNode, String testNode) {
    stage(htBuild.getName()) {
        boolean sameNode = testNode == buildNode
        try {
            node(buildNode) {
                stage(htBuild.getName() + ': checkout') {
                    checkout scm
                }
                stage(htBuild.getName() + ': configure') {
                    htBuild.configure()
                }
                stage(htBuild.getName() + ': build') {
                    htBuild.build()
                }
                if (!sameNode) htBuild.stashBuildArtifacts(testNode + '-build-artifacts')
            }
            node (testNode) {
                if (!sameNode) unstash testNode + '-build-artifacts'
                if (htBuild.testsEnabled) {
                    stage(htBuild.getName() + ': run tests') {
                        htBuild.runTests()
                    }
                }
                if (htBuild.benchmarksEnabled) {
                    stage(htBuild.getName() + ': run benchmarks') {
                        htBuild.runBenchmarks()
                    }
                }
                if (!sameNode) {
                    htBuild.stashReports(htBuild.getName() + '-reports')
                    deleteDir()
                }
            }
            node (buildNode) {
                if (!sameNode) unstash htBuild.getName() + '-reports'
                htBuild.publishReports()
            }
        } catch (exc) {
            echo 'Build failed: ' + exc.toString()
            echo 'Message: ' + exc.getMessage()
            echo 'CallStack: ' + exc.getStackTrace()
            error("Build failed.")
        } finally {
            node(buildNode) {
                deleteDir()
            }
        }
    }
}

abstract class HTBuild {
    protected String cmakeArgs = ' -DENABLE_EXAMPLES=ON -DENABLE_BENCHMARKS=ON'
    protected final context
    protected boolean coverageEnabled
    protected String name
    public boolean testsEnabled
    public boolean benchmarksEnabled

    HTBuild(context, name) {
        this.context = context
        this.coverageEnabled = false
        this.testsEnabled = false
        this.benchmarksEnabled = false
	this.name = name
    }

    def getName() {
        return name
    }

    def withToolchain(String toolchainPath) {
        cmakeArgs += ' -DCMAKE_TOOLCHAIN_FILE=' + toolchainPath
        return this
    }

    def withInitialCacheFile(String cacheFile) {
        cmakeArgs += ' -C ' + cacheFile
        return this
    }

    def withDebugBuild() {
        cmakeArgs += ' -DCMAKE_BUILD_TYPE=Debug'
        return this
    }

    def withBenchmarks() {
        cmakeArgs += ' -DENABLE_BENCHMARKS=ON'
        this.benchmarksEnabled = true
        return this
    }

    def withCodeCoverage() {
        cmakeArgs += ' -DENABLE_CODE_COVERAGE=ON'
        coverageEnabled = true
        return this
    }

    def withTests() {
        cmakeArgs += ' -DENABLE_TESTS=ON'
        testsEnabled = true
        return this
    }

    def withAsan() {
        cmakeArgs += ' -DENABLE_ASAN=ON'
        return this
    }

    def configure() {
        context.sh 'cmake ' + this.cmakeArgs + ' .'
    }

    def publishReports() {
        context.stage(name + ': publish reports') {
            if (testsEnabled) {
                context.junit 'tests/hawktracer_tests.xml'
            }
            if (coverageEnabled) {
                context.step([$class: 'CoberturaPublisher', coberturaReportFile: 'coverage.xml'])
            }
            if (benchmarksEnabled) {
                // TODO publish benchmark results
            }
        }
    }

    def stashBuildArtifacts(String stashName) {}
    def stashReports(String stashName) {}

    abstract def build()

    abstract def runTests()

    abstract def runBenchmarks()
}

@InheritConstructors
class LinuxBuild extends HTBuild {
    def build() {
        context.sh 'make -j 2'
    }

    def runTests() {
        context.sh 'make test CTEST_OUTPUT_ON_FAILURE=1'

        if (coverageEnabled) {
            context.sh 'gcovr -r . --exclude-directories tests --exclude-directories benchmarks --exclude-directories examples --xml -o coverage.xml'
        }
    }

    def runBenchmarks() {
        context.stage(name + ': run benchmarks') {
            context.sh 'LD_LIBRARY_PATH=./lib ./benchmarks/hawktracer_benchmarks --benchmark_format=json --benchmark_out=benchmark.json'
        }
    }

    def stashBuildArtifacts(String stashName) {
        String buildArtifacts = 'lib/libhawktracer.so,'
        if (benchmarksEnabled) {
            buildArtifacts += 'benchmarks/hawktracer_benchmarks,'
        }
        if (testsEnabled) {
            buildArtifacts += 'tests/hawktracer_tests,'
            buildArtifacts += 'tests/hawktracer_test_destroy_timeline_after_uninit,'
        }

        context.stash includes: buildArtifacts, name: stashName
    }

    def stashReports(String stashName) {
        String reports = ''

        if (benchmarksEnabled) {
            reports += 'benchmark.json,'
        }

        if (testsEnabled) {
            reports += 'tests/hawktracer_tests.xml'
        }

        context.stash includes: reports, name: stashName
    }
}

@InheritConstructors
class RaspberryBuild extends LinuxBuild {
    @Override
    def runTests() {
        context.sh 'LD_LIBRARY_PATH=./lib ./tests/hawktracer_tests --gtest_output=xml:tests/hawktracer_tests.xml'
        context.sh 'LD_LIBRARY_PATH=./lib ./tests/hawktracer_test_destroy_timeline_after_uninit'
    }
}
