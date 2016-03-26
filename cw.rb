
EAW = { 'A' => 1, 'N' => 1, 'F' => 2, 'H' => 1, 'W' => 2, 'Na' => 1 }
GCAT = { 'Cc' => -1, 'Cf' => -1, 'Cs' => -1, 'Co' => -1, 'Cn' => -1, nil => :eaw, 'Mn' => 0 }

def cw (ud_fn, eaw_fn, b)
  b = b.to_i
  q = []
  IO.readlines(ud_fn).each do |l|
    l = l.chomp.split('#', 2)[0]
    next if l.nil? || l.empty?
    l.strip!
    cp, name, gen_cat, can_comb_class, bidi_class, *r = *l.split(';')
    cp = cp.hex
    q[cp] = {:name => name, :gen_cat => gen_cat, :can_comb_class => can_comb_class, :bidi_class => bidi_class}
  end # IO.readlines(ud).each do |l|

  IO.readlines(eaw_fn).each do |l|
    l = l.chomp.split('#', 2)[0]
    next if l.nil? || l.empty?
    #puts "---" + l
    l.strip!
    cp, eaw = *l.split(';')
    if cp.index('..')
      cpl, cph = *cp.split('..').map{|x| x.hex}
    else
      cpl = cp.hex
      cph = cpl
    end # if cp.index('..')
    (cpl..cph).each do |cp|
      q[cp] ||= {}
      q[cp][:eaw] = eaw
    end # (cpl..cph).each |cp}
  end # IO.readlines(eaw_fn).each do |l|

  v = 0
  sh = 0
  s = []
  h = {}
  dtab = []
  otab = []

  mp = (1 << b) - 1

  (0...0x110000).each do |cp|
  #q.each do |cp|
    #next unless q[cp]
    if q[cp]
      w = GCAT[q[cp][:gen_cat]] || :eaw
      w = EAW[q[cp][:eaw]] if w == :eaw
    else
      w = -1
    end
    #STDERR.puts "%06X %u" % [cp, w]
    v |= ((w + 1) << sh)
    sh += 2
    next unless sh == 8
    s << v
    if (cp & mp) == mp
      if !h[s]
        h[s] = dtab.length
        dtab << s
      end # if h[s]
      otab << h[s]
      s = []
    end # if (cp & 0x7F) == 0x7F
    sh = 0
    v = 0
    #puts "%06X %2s %2s %d" % [cp, q[cp][:gen_cat], q[cp][:eaw], w]
  end # q.each do |cp|

  out = "/* GENERATED FILE - DO NOT (BOTHER TO) EDIT */\n"
  out << "#include <stdint.h>\n"
  out << "uint8_t zlx_ucp_width_ofs_a[] = {\n"
  otab.each_with_index do |o, i|
    out << "    " if (i & 15) == 0
    out << ("%3d," % o)
    out << "\n" if (i & 15) == 15
  end # otab.each_with_index do |o, i|
  out << "};\n"

  out << "uint8_t zlx_ucp_width_val_a[] = {\n"
  dtab.each_with_index do |dt, i|
    m = dt.map{|d| ("%3d," % d)}
    4.times {|i|
        out << "    "
        out << m[i * 0x10, 0x10].join
        out << "\n"
    }
    out << "\n"
  end # dtab.each_with_index do |o, i|
  out << "};\n"
  return out
end # def cw (ud, eaw)

puts cw(*ARGV)

