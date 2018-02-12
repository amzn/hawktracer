import groovy.transform.InheritConstructors

node('ignite-linux') {
    runBuild(new LinuxBuild(this).withDebugBuild().withCodeCoverage().withTests().withAsan(), 'Linux')
}

def runBuild(HTBuild htBuild, String name) {
    try {
        stage(name + ': checkout') {
            checkout scm
        }
        stage(name + ': configure') {
            htBuild.configure()
        }
        stage(name + ': build') {
            htBuild.build()
        }
        stage(name + ': run tests') {
            htBuild.runTests()
        }
        stage(name + ': publish reports') {
            htBuild.publishReports()
        }
    }
    catch (exc) {
        echo 'Build failed: ' + exc.toString()
        echo 'Message: ' + exc.getMessage()
        echo 'CallStack: ' + exc.getStackTrace()

        deleteDir()
    }
}

abstract class HTBuild {
    protected String cmakeArgs = ' -DENABLE_EXAMPLES=ON -DENABLE_BENCHMARKS=ON'
    protected final context
    protected boolean coverageEnabled
    protected boolean testsEnabled

    HTBuild(context) {
        this.context = context
        this.coverageEnabled = false
        this.testsEnabled = false
    }

    def withDebugBuild() {
        cmakeArgs += ' -DCMAKE_BUILD_TYPE=Debug'
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
        if (testsEnabled) {
            context.junit 'tests/hawktracer_tests.xml'
        }
        if (coverageEnabled) {
            context.step([$class: 'CoberturaPublisher', coberturaReportFile: 'coverage.xml'])
        }
    }

    abstract def build()

    abstract def runTests()
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
}
