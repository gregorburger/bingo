{
  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-25.11";

  outputs = { self, nixpkgs }:
    let
      pkgs = nixpkgs.legacyPackages.x86_64-linux;
      pkgsWindows = import nixpkgs {
        system = "x86_64-linux";
        crossSystem = {
          config = "x86_64-w64-mingw32";
        };
      };
      qtEnv = pkgs.qt6.env "qt6-dev-${pkgs.qt6.qtbase.version}" [
        pkgs.qt6.qtbase
        pkgs.qt6.qtsvg
        pkgs.qt6.qtdeclarative
        pkgs.qt6.qtwebengine
        pkgs.qt6.qtwebchannel
        pkgs.qt6.qttools
        pkgs.qt6.qtimageformats
        pkgs.qt6.qtpositioning
        # add/remove as needed
      ];
    in {
      packages.x86_64-linux.bingo-windows = pkgsWindows.stdenv.mkDerivation {
        pname = "bingo";
        version = "1.0";

        src = ./.;

        nativeBuildInputs = with pkgs; [
          cmake
          ninja
        ];

        buildInputs = with pkgsWindows; [
          qt6.qtbase
          qt6.qtsvg
          qt6.qtdeclarative
          qt6.qttools
          boost
        ];

        cmakeFlags = [
          "-DCMAKE_BUILD_TYPE=Release"
        ];

        installPhase = ''
          mkdir -p $out/bin
          cp bingo.exe $out/bin/

          # Copy Qt6 DLLs
          for dll in ${pkgsWindows.qt6.qtbase}/bin/*.dll; do
            cp $dll $out/bin/ || true
          done
          for dll in ${pkgsWindows.qt6.qtsvg}/bin/*.dll; do
            cp $dll $out/bin/ || true
          done
          for dll in ${pkgsWindows.qt6.qtdeclarative}/bin/*.dll; do
            cp $dll $out/bin/ || true
          done

          # Copy MinGW runtime DLLs
          cp ${pkgsWindows.windows.mingw_w64_pthreads}/bin/*.dll $out/bin/ || true
          cp ${pkgsWindows.stdenv.cc.cc}/x86_64-w64-mingw32/lib/*.dll $out/bin/ || true

          # Copy Qt plugins
          mkdir -p $out/bin/platforms
          cp ${pkgsWindows.qt6.qtbase}/plugins/platforms/*.dll $out/bin/platforms/ || true
        '';
      };

      devShells.x86_64-linux.default = pkgs.mkShell {
        buildInputs = [
          qtEnv
          pkgs.cmake
          pkgs.ninja
          pkgs.gcc
          pkgs.gdb
          pkgs.boost
          pkgs.libGL
          pkgs.clazy
          #pkgs.qt6.wrapQtAppsHook
        ];

        shellHook = ''
          export QT_PLUGIN_PATH="${qtEnv}/lib/qt-6/plugins"
          export QML_IMPORT_PATH="${qtEnv}/lib/qt-6/qml"
          export QT_QPA_PLATFORM_PLUGIN_PATH="${qtEnv}/lib/qt-6/plugins/platforms"
          export QT_QPA_PLATFORM="xcb"
        '';
      };

      devShells.x86_64-linux.windows = pkgs.mkShell {
        nativeBuildInputs = with pkgs; [
          cmake
          ninja
          pkgsCross.mingwW64.stdenv.cc
        ];

        buildInputs = with pkgsWindows; [
          qt6.qtbase
          qt6.qtsvg
          qt6.qtdeclarative
          qt6.qttools
          boost
        ];

        shellHook = ''
          echo "Windows cross-compilation environment loaded"
          echo "Use: mkdir build-win && cd build-win"
          echo "     cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-mingw.cmake .."
          echo "     cmake --build ."
        '';
      };
    };
}