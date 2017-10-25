#!/usr/bin/env python

"""
script to build the latest binaries for each vehicle type, ready to upload
Peter Barker, August 2017
based on build_binaries.sh by Andrew Tridgell, March 2013
"""

from __future__ import print_function

import datetime
import distutils.dir_util
import optparse
import os
import re
import shutil
import time
import subprocess
import sys
import zlib

# local imports
import generate_manifest


class build_binaries(object):
    def __init__(self, tags):
        self.tags = tags
        self.dirty = False

    def progress(self, string):
        '''pretty-print progress'''
        print("BB: %s" % string)

    def run_git(self, args):
        '''run git with args git_args; returns git's output'''
        cmd_list = ["git"]
        cmd_list.extend(args)
        return self.run_program("BB-GIT", cmd_list)

    def board_branch_bit(self, board):
        '''return a fragment which might modify the branch name'''
        if board in ["apm1", "apm2"]:
            return "-AVR"
        return None

    def board_options(self, board):
        '''return board-specific options'''
        if board == "bebop":
            return ["--static"]
        return []

    def run_waf(self, args):
        if os.path.exists("waf"):
            waf = "./waf"
        else:
            waf = os.path.join(".", "modules", "waf", "waf-light")
        cmd_list = [waf]
        cmd_list.extend(args)
        self.run_program("BB-WAF", cmd_list)

    def run_program(self, prefix, cmd_list):
        self.progress("Running (%s)" % " ".join(cmd_list))
        p = subprocess.Popen(cmd_list, bufsize=1, stdin=None,
                             stdout=subprocess.PIPE, close_fds=True,
                             stderr=subprocess.STDOUT)
        output = ""
        while True:
            x = p.stdout.readline()
            if len(x) == 0:
                if os.waitpid(p.pid, 0):
                    break
                    # select not available on Windows... probably...
                    time.sleep(0.1)
                    continue
            output += x
            x = x.rstrip()
            print("%s: %s" % (prefix, x))
        return output

    def run_make(self, args):
        cmd_list = ["make"]
        cmd_list.extend(args)
        self.run_program("BB-MAKE", cmd_list)

    def run_git_update_submodules(self):
        '''if submodules are present initialise and update them'''
        if os.path.exists(os.path.join(self.basedir, ".gitmodules")):
            self.run_git(["submodule",
                          "update",
                          "--init",
                          "--recursive",
                          "-f"])

    def checkout(self, vehicle, ctag, cboard=None, cframe=None):
        '''attempt to check out a git tree.  Various permutations are
attempted based on ctag - for examplle, if the board is avr and ctag
is bob we will attempt to checkout bob-AVR'''
        if self.dirty:
            self.progress("Skipping checkout for dirty build")
            return True

        self.progress("Trying checkout %s %s %s %s" %
                      (vehicle, ctag, cboard, cframe))
        self.run_git(['stash'])
        if ctag == "latest":
            vtag = "master"
        else:
            vtag = "%s-%s" % (vehicle, ctag)

        branches = []
        if cframe is not None:
            # try frame specific tag
            branches.append("%s-%s" % (vtag, cframe))
        if cboard is not None:
            bbb = self.board_branch_bit(cboard)
            if bbb is not None:
                # try board type specific branch extension
                branches.append("".join([vtag, bbb]))
        branches.append(vtag)

        for branch in branches:
            try:
                self.progress("Trying branch %s" % branch)
                self.run_git(["checkout", "-f", branch])
                self.run_git_update_submodules()
                self.run_git(["log", "-1"])
                return True
            except subprocess.CalledProcessError as e:
                self.progress("Checkout branch %s failed" % branch)
                pass

        self.progress("Failed to find tag for %s %s %s %s" %
                      (vehicle, ctag, cboard, cframe))
        return False

    def skip_board_waf(self, board):
        '''check if we should skip this build because we don't support the
        board in this release
        '''

        try:
            if self.string_in_filepath(board,
                                       os.path.join(self.basedir,
                                                    'Tools',
                                                    'ardupilotwaf',
                                                    'boards.py')):
                return False
        except IOError as e:
            if e.errno != 2:
                raise
        self.progress("Skipping unsupported board %s" % (board,))
        return True

    def skip_frame(self, board, frame):
        '''returns true if this board/frame combination should not be built'''
        if frame == "heli":
            if board in ["bebop", "aerofc-v1"]:
                self.progress("Skipping heli build for %s" % board)
                return True
        return False

    def first_line_of_filepath(self, filepath):
        '''returns the first (text) line from filepath'''
        with open(filepath) as fh:
            line = fh.readline()
        return line

    def skip_build(self, buildtag, builddir):
        '''check if we should skip this build because we have already built
        this version
        '''

        if os.getenv("FORCE_BUILD", False):
            return False

        if not os.path.exists(os.path.join(self.basedir, '.gitmodules')):
            self.progress("Skipping build without submodules")
            return True

        bname = os.path.basename(builddir)
        ldir = os.path.join(os.path.dirname(os.path.dirname(
            os.path.dirname(builddir))), buildtag, bname)  # FIXME: WTF

        oldversion_filepath = os.path.join(ldir, "git-version.txt")
        if not os.path.exists(oldversion_filepath):
            self.progress("%s doesn't exist - building" % oldversion_filepath)
            return False

        oldversion = self.first_line_of_filepath(oldversion_filepath)
        newversion = self.run_git(["log", "-1"])
        newversion = newversion.splitlines()[0]
        oldversion = oldversion.rstrip()
        newversion = newversion.rstrip()
        self.progress("oldversion=%s newversion=%s" %
                      (oldversion, newversion,))
        if oldversion == newversion:
            self.progress("Skipping build - version match (%s)" %
                          (newversion,))
            return True

        self.progress("%s needs rebuild" % (ldir,))
        return False

    def write_string_to_filepath(self, string, filepath):
        '''writes the entirety of string to filepath'''
        with open(filepath, "w") as x:
            x.write(string)

    def addfwversion_gitversion(self, destdir, src):
        # create git-version.txt:
        gitlog = self.run_git(["log", "-1"])
        gitversion_filepath = os.path.join(destdir, "git-version.txt")
        gitversion_content = gitlog
        versionfile = os.path.join(src, "version.h")
        if os.path.exists(versionfile):
            content = self.read_string_from_filepath(versionfile)
            match = re.search('define.THISFIRMWARE "([^"]+)"', content)
            if match is None:
                self.progress("Failed to retrieve THISFIRMWARE from version.h")
                self.progress("Content: (%s)" % content)
            self.progress("Writing version info to %s" % (gitversion_filepath,))
            gitversion_content += "\nAPMVERSION: %s\n" % (match.group(1))
        else:
            self.progress("%s does not exist" % versionfile)

        self.write_string_to_filepath(gitversion_content, gitversion_filepath)

    def addfwversion_firmwareversiontxt(self, destdir, src):
        # create firmware-version.txt
        versionfile = os.path.join(src, "version.h")
        if not os.path.exists(versionfile):
            self.progress("%s does not exist" % (versionfile,))
            return;
        ss = ".*define +FIRMWARE_VERSION[	 ]+(?P<major>\d+)[ ]*,[ 	]*" \
             "(?P<minor>\d+)[ ]*,[	 ]*(?P<point>\d+)[ ]*,[	 ]*" \
             "(?P<type>[A-Z_]+)[	 ]*"
        content = self.read_string_from_filepath(versionfile)
        match = re.search(ss, content)
        if match is None:
            self.progress("Failed to retrieve FIRMWARE_VERSION from version.h")
            self.progress("Content: (%s)" % content)
            return
        ver = "%d.%d.%d-%s\n" % (int(match.group("major")),
                                 int(match.group("minor")),
                                 int(match.group("point")),
                                 match.group("type"))
        firmware_version_filepath = "firmware-version.txt"
        self.progress("Writing version (%s) to %s" %
                      (ver, firmware_version_filepath,))
        self.write_string_to_filepath(
            ver, os.path.join(destdir, firmware_version_filepath))

    def addfwversion(self, destdir, src):
        '''write version information into destdir'''
        self.addfwversion_gitversion(destdir, src)
        self.addfwversion_firmwareversiontxt(destdir, src)

    def read_string_from_filepath(self, filepath):
        '''returns content of filepath as a string'''
        with open(filepath, 'rb') as fh:
            content = fh.read()
        return content

    def string_in_filepath(self, string, filepath):
        '''returns true if string exists in the contents of filepath'''
        return string in self.read_string_from_filepath(filepath)

    def copyit(self, afile, adir, tag, src):
        '''copies afile into various places, adding metadata'''
        bname = os.path.basename(adir)
        tdir = os.path.join(os.path.dirname(os.path.dirname(
            os.path.dirname(adir))), tag, bname)
        if tag == "latest":
            # we keep a permanent archive of all "latest" builds,
            # their path including a build timestamp:
            distutils.dir_util.mkpath(adir)
            self.progress("Copying $file to $adir")
            shutil.copy(afile, adir)
            self.addfwversion(adir, src)
        # the most recent build of every tag is kept around:
        self.progress("Copying %s to %s" % (afile, tdir))
        distutils.dir_util.mkpath(tdir)
        self.addfwversion(tdir, src)
        shutil.copy(afile, tdir)

    def touch_filepath(self, filepath):
        '''creates a file at filepath, or updates the timestamp on filepath'''
        if os.path.exists(filepath):
            os.utime(filepath, None)
        else:
            with open(filepath, "a"):
                pass

    def build_vehicle_apm(self, tag, vehicle, board,
                          vehicle_binaries_subdir, binaryname):
        self.progress("Building %s %s %s binaries" % (vehicle, tag, board))
        if not self.checkout(vehicle, tag, board):
            self.progress("Failed checkout of %s %s %s" %
                          (vehicle, board, tag))
            self.error_count += 1
            return
        framesuffix = ""
        ddir = os.path.join(self.binaries,
                            vehicle_binaries_subdir,
                            self.hdate_ym,
                            self.hdate_ymdhm,
                            "".join([board, framesuffix]))
        if self.skip_build(tag, ddir):
            return

        self.run_make(["-C", vehicle, "clean"])
        self.run_make(["-C", vehicle, "-j4", board])

        binaryname = vehicle  # HACK!  make targets are mixed-case
        path = os.path.join(self.tmpdir,
                            "".join([vehicle, ".build"]),
                            "".join([binaryname, framesuffix, ".hex"]))
        self.copyit(path, ddir, tag, vehicle)
        self.touch_filepath(os.path.join(self.binaries,
                                         vehicle_binaries_subdir, tag))

    def build_vehicle(self, tag, vehicle, boards, vehicle_binaries_subdir,
                      binaryname, px4_binaryname, frames=[None]):
        '''build vehicle binaries'''
        self.progress("Building %s %s binaries (cwd=%s)" %
                      (vehicle, tag, os.getcwd()))
        # if not self.checkout(vehicle, tag):
        #     self.progress("Failed to check out (%s)" % tag)
        #     return

        # # begin pointless checkout
        # if not self.checkout(vehicle, "latest"):
        #     self.progress("Failed to check out (%s)" % "latest")
        #     return
        # # end pointless checkout

        for board in boards:
            if "apm" in board:
                # apm does't do frames
                self.build_vehicle_apm(tag, vehicle, board,
                                       vehicle_binaries_subdir, binaryname)
                continue
            self.progress("Building board: %s" % board)
            for frame in frames:
                if frame is not None:
                    self.progress("Considering frame %s for board %s" %
                                  (frame, board))
                if frame is None:
                    framesuffix = ""
                else:
                    framesuffix = "-%s" % frame
                if not self.checkout(vehicle, tag, board, frame):
                    self.progress("Failed checkout of %s %s %s %s" %
                                  (vehicle, board, tag, frame))
                    self.error_count += 1
                    continue
                if self.skip_board_waf(board):
                    continue
                self.progress("Building %s %s %s binaries %s" %
                              (vehicle, tag, board, frame))
                ddir = os.path.join(self.binaries,
                                    vehicle_binaries_subdir,
                                    self.hdate_ym,
                                    self.hdate_ymdhm,
                                    "".join([board, framesuffix]))
                if self.skip_build(tag, ddir):
                    continue
                if self.skip_frame(board, frame):
                    continue
                self.progress("Configuring for %s in %s" %
                              (board, self.buildroot))
                try:
                    waf_opts = ["configure",
                                "--board", board,
                                "--out", self.buildroot,
                                "clean"]
                    waf_opts.extend(self.board_options(board))
                    self.run_waf(waf_opts)
                except subprocess.CalledProcessError as e:
                    self.progress("waf configure failed")
                    continue
                try:
                    target = os.path.join("bin",
                                          "".join([binaryname, framesuffix]))
                    self.run_waf(["build", "--targets", target])
                except subprocess.CalledProcessError as e:
                    self.progress("Failed build of %s %s%s %s" %
                                  (vehicle, board, framesuffix, tag))
                    self.error_count += 1
                    continue

                bare_path = os.path.join(self.buildroot,
                                         board,
                                         "bin",
                                         "".join([binaryname, framesuffix]))
                px4_path = "".join([bare_path, ".px4"])
                if os.path.exists(px4_path):
                    path = px4_path
                else:
                    path = bare_path
                self.copyit(path, ddir, tag, vehicle)
                # why is touching this important? -pb20170816
                self.touch_filepath(os.path.join(self.binaries,
                                                 vehicle_binaries_subdir, tag))

        # PX4-building
        board = "px4"
        for frame in frames:
            self.progress("Building frame %s for board %s" % (frame, board))
            if frame is None:
                framesuffix = ""
            else:
                framesuffix = "-%s" % frame

            if not self.checkout(vehicle, tag, "PX4", frame):
                self.progress("Failed checkout of %s %s %s %s" %
                              (vehicle, "PX4", tag, frame))
                self.error_count += 1
                self.checkout(vehicle, "latest")
                continue

            try:
                deadwood = "../Build.%s" % vehicle
                if os.path.exists(deadwood):
                    shutil.rmtree(os.path.join(deadwood))
            except Exception as e:
                self.progress("FIXME: narrow exception (%s)" % repr(e))

            self.progress("Building %s %s PX4%s binaries" %
                          (vehicle, tag, framesuffix))
            ddir = os.path.join(self.binaries,
                                vehicle_binaries_subdir,
                                self.hdate_ym,
                                self.hdate_ymdhm,
                                "".join(["PX4", framesuffix]))
            if self.skip_build(tag, ddir):
                continue

            for v in ["v1", "v2", "v3", "v4", "v4pro"]:
                px4_v = "%s-%s" % (board, v)

                if self.skip_board_waf(px4_v):
                    continue

                self.progress("Configuring for %s in %s" %
                              (px4_v, self.buildroot))
                try:
                    self.run_waf(["configure", "--board", px4_v,
                                  "--out", self.buildroot, "clean"])
                except subprocess.CalledProcessError as e:
                    self.progress("waf configure failed")
                    continue
                try:
                    self.run_waf([
                        "build",
                        "--targets",
                        os.path.join("bin",
                                     "".join([binaryname, framesuffix]))])
                except subprocess.CalledProcessError as e:
                    self.progress("Failed build of %s %s%s %s for %s" %
                                  (vehicle, board, framesuffix, tag, v))
                    self.error_count += 1
                    continue

                oldfile = os.path.join(self.buildroot, px4_v, "bin",
                                       "%s%s.px4" % (binaryname, framesuffix))
                newfile = "%s-%s.px4" % (px4_binaryname, v)
                self.progress("Copying (%s) to (%s)" % (oldfile, newfile,))
                try:
                    shutil.copyfile(oldfile, newfile)
                except Exception as e:
                    self.progress("FIXME: narrow exception (%s)" % repr(e))
                    self.progress("Failed build copy of %s PX4%s %s for %s" %
                                  (vehicle, framesuffix, tag, v))
                    self.error_count += 1
                    continue
                # FIXME: why the two stage copy?!
                self.copyit(newfile, ddir, tag, vehicle)
        self.checkout(vehicle, "latest")

    def common_boards(self):
        '''returns list of boards common to all vehicles'''
        # note that while we do not use these for AntennaTracker!
        return ["erlebrain2", "navio", "navio2", "pxf", "pxfmini"]

    def build_arducopter(self, tag):
        '''build Copter binaries'''
        boards = self.common_boards()[:]
        boards.extend(["aerofc-v1", "bebop"])
        self.build_vehicle(tag,
                           "ArduCopter",
                           boards,
                           "Copter",
                           "arducopter",
                           "ArduCopter",
                           frames=[None, "heli"])

    def build_arduplane(self, tag):
        '''build Plane binaries'''
        boards = self.common_boards()[:]
        boards.append("disco")
        self.build_vehicle(tag,
                           "ArduPlane",
                           boards,
                           "Plane",
                           "arduplane",
                           "ArduPlane")

    def build_antennatracker(self, tag):
        '''build Tracker binaries'''
        boards = ['navio', 'navio2']
        boards.append('apm2')
        self.build_vehicle(tag,
                           "AntennaTracker",
                           boards,
                           "AntennaTracker",
                           "antennatracker",
                           "AntennaTracker",)

    def build_rover(self, tag):
        '''build Rover binaries'''
        boards = self.common_boards()
        boards.extend(['apm1', 'apm2'])
        self.build_vehicle(tag,
                           "APMrover2",
                           boards,
                           "Rover",
                           "ardurover",
                           "APMrover2")

    def build_ardusub(self, tag):
        '''build Sub binaries'''
        self.build_vehicle(tag,
                           "ArduSub",
                           self.common_boards(),
                           "Sub",
                           "ardusub",
                           "ArduSub")

    def generate_manifest(self):
        '''generate manigest files for GCS to download'''
        self.progress("Generating manifest")
        base_url = 'http://firmware.ardupilot.org'
        generator = generate_manifest.ManifestGenerator(self.binaries,
                                                        base_url)
        content = generator.json()
        new_json_filepath = os.path.join(self.binaries, "manifest.json.new")
        self.write_string_to_filepath(content, new_json_filepath)
        # provide a pre-compressed manifest.  For reference, a 7M manifest
        # "gzip -9"s to 300k in 1 second, "xz -e"s to 80k in 26 seconds
        compressed = zlib.compress(content, 9)
        new_json_filepath_gz = os.path.join(self.binaries,
                                            "manifest.json.gz.new")
        self.write_string_to_filepath(compressed, new_json_filepath_gz)
        json_filepath = os.path.join(self.binaries, "manifest.json")
        json_filepath_gz = os.path.join(self.binaries, "manifest.json.gz")
        shutil.move(new_json_filepath, json_filepath)
        shutil.move(new_json_filepath_gz, json_filepath_gz)
        self.progress("Manifest generation successful")

    def validate(self):
        '''run pre-run validation checks'''
        if "dirty" in self.tags:
            if len(self.tags) > 1:
                raise ValueError("dirty must be only tag if present (%s)" %
                                 (str(self.tags)))
            self.dirty = True

    def pollute_env_from_file(self, filepath):
        with open(filepath) as f:
            for line in f:
                (name, value) = str.split(line, "=")
                value = value.rstrip()
                self.progress("%s: %s=%s" % (filepath, name,value))
                os.environ[name] = value
        sys.exit(1)

    def run(self):
        self.validate()

        prefix_bin_dirpath = os.path.join(os.environ.get('HOME'),
                                          "prefix", "bin")
        origin_env_path = os.environ.get("PATH")
        os.environ["PATH"] = ':'.join([prefix_bin_dirpath, origin_env_path,
                                       "/bin", "/usr/bin"])
        self.tmpdir = os.path.join(os.getcwd(), 'build.tmp.binaries')
        os.environ["TMPDIR"] = self.tmpdir

        print(self.tmpdir)
        if os.path.exists(self.tmpdir):
            self.progress("Removing (%s)" % (self.tmpdir,))
            shutil.rmtree(self.tmpdir)

        self.progress("Building in %s" % self.tmpdir)

        now = datetime.datetime.now()
        self.progress(now)

        if not self.dirty:
            self.run_git(["checkout", "-f", "master"])
        githash = self.run_git(["rev-parse", "HEAD"])
        githash = githash.rstrip()
        self.progress("git hash: %s" % str(githash))

        self.hdate_ym = now.strftime("%Y-%m")
        self.hdate_ymdhm = now.strftime("%Y-%m-%d-%H:%m")

        distutils.dir_util.mkpath(os.path.join("binaries", self.hdate_ym,
                                               self.hdate_ymdhm))
        self.binaries = os.path.join(os.getcwd(), "..", "buildlogs",
                                     "binaries")
        self.basedir = os.getcwd()
        self.error_count = 0

        if os.path.exists("config.mk"):
            # FIXME: narrow exception
            self.pollute_env_from_file("config.mk")

        if not self.dirty:
            self.run_git_update_submodules()
        self.buildroot = os.path.join(os.environ.get("TMPDIR"),
                                      "binaries.build")
        if os.path.exists(self.buildroot):
            shutil.rmtree(self.buildroot)

        for tag in self.tags:
            self.build_arducopter(tag)
            self.build_arduplane(tag)
            self.build_rover(tag)
            self.build_antennatracker(tag)
            self.build_ardusub(tag)

        if os.path.exists(self.tmpdir):
            shutil.rmtree(self.tmpdir)

        self.generate_manifest()

        sys.exit(self.error_count)


if __name__ == '__main__':
    parser = optparse.OptionParser("build_binaries.py")

    parser.add_option("", "--tags", action="append", type="string",
                      default=[], help="tags to build")
    cmd_opts, cmd_args = parser.parse_args()

    tags = cmd_opts.tags
    if len(tags) == 0:
        # FIXME: wedge this defaulting into parser somehow
        tags = ["stable", "beta", "latest"]

    bb = build_binaries(tags)
    bb.run()
