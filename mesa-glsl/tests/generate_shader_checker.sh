#!/bin/sh

OUTPUT_PATH=$1
TEST_DIR=$2

cat <<EOF
#!/bin/sh
SHADERS_PATH="${OUTPUT_PATH}/shaders"
FILES_PATH="${TEST_DIR}/shaders"
mkdir -p \${SHADERS_PATH}
rm -f \${SHADERS_PATH}/*.{frag,geom}
for type in geom frag; do
  ( cd \$FILES_PATH/test.\${type}.dbgout
    for file in \$(find . -type f ! -name rules); do
      if grep --quiet "^no code$" \${file}; then continue; fi
      sed '/^==/d' \${file} > \${SHADERS_PATH}/\${file}.\${type}
    done )
done
EOF
