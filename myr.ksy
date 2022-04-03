meta:
  id: myr
  application: Myr Archive
  endian: le
seq:
  - id: magic
    contents: ["MyrA"]
  - id: version
    type: u1
    size: 1
  - id: entry_count
    type: u8
  - id: entry_metas
    type: entry_meta
    repeat: expr
    repeat-expr: entry_count

types:
  entry_meta:
    seq:
      - id: name
        type: strz
        encoding: ASCII
      - id: file_pos
        type: u4
      - id: file_size
        type: u4
    instances:
      data:
        pos: file_pos
        size: file_size
        io: _root._io