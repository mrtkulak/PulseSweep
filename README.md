# PulseSweep 1.1 — macOS derleme paketi

Bu paket hazır bir .bundle içermez. Windows ortamında hazırlanmış, Mac'te derlenmesi ve denenmesi gereken kaynak paketidir.

## Korunan özellikler

src klasöründeki üç dosya Windows PulseSweep 1.1 kaynaklarıyla bayt düzeyinde aynıdır. Dalga motoru, üç mod, renk seçici, varsayılanlar, tetikleme ve shader değiştirilmedi. Windows DLL'sine dokunulmadı.

## Mac'te derleme

1. ZIP'i Mac'te aç.
2. Apple Xcode Command Line Tools yüklü değilse Terminal'de `xcode-select --install` çalıştırıp kurulumu tamamla.
3. Terminal'e `bash ` yaz, `build-macos.command` dosyasını Terminal'e sürükle ve Enter'a bas.

Betik Apple Silicon (arm64) ve Intel (x86_64) içeren universal **PulseSweep.bundle** üretmeyi hedefler. Minimum hedef macOS 11.0; derleyici/SDK iki mimariyi desteklemelidir. Homebrew, CMake veya GLEW kurulumu gerekmez. Kaynak bağımlılıkları pakettedir, betik indirme yapmaz.

Çıktı yeni oluşturulan `build/mac-universal.XXXXXX/` klasöründedir. Betik iki mimariyi, plist'i, yerel imzayı, dalga motorunu ve çalıştığı Mac'in mimarisinde bundle yükleme/parametre bilgilerini denetler. Ayrıca .bundle'ı ZIP olarak paketler.

## Resolume'a ekleme

Derleme başarılı olduktan sonra **PulseSweep.bundle klasörünün tamamını** Resolume'un taradığı FFGL eklenti klasörüne kopyala veya bundle'ın bulunduğu üst klasörü Preferences > Video üzerinden ekle. Resolume'u yeniden başlatıp Effects içinde PulseSweep ara.

Yerel ad-hoc imza kullanılır; Apple Developer ID ile imzalanmış/noterlenmiş bir dağıtım değildir.

## Tamamlanması gereken testler

- Bu paket henüz Apple derleyicisiyle derlenmedi; gerçek .bundle üretilmedi.
- Bundle kontrolü OpenGL render testi değildir. Mac Resolume'da üç mod, tek Flash Color seçicisi, renkler, üst üste tetikleme ve saydamlık denenmelidir.
- Universal derleme iki mimariyi içerir; otomatik yükleme testi yalnızca çalıştırılan Mac'in mimarisinde yürür. Diğer mimarinin çalışma testi ayrı gerekir.
- Aynı kaynak kodu platformlar arasında tamamen aynı davranışı garanti etmez; sürücü/host farkları bu testlerle kontrol edilmelidir.

FFGL kimliği MWA1, görünen ad PulseSweep, sürüm 1.1. Başka isimli eski kopyayla aynı anda kurulması önerilmez.

Lisans: özgün kod ve betikler LICENSE.txt; FFGL SDK vendor/ffgl/LICENSE.md ve kaynak başlıkları.
Referanslar: https://github.com/resolume/ffgl ve https://developer.apple.com/documentation/apple-silicon/building-a-universal-macos-binary
