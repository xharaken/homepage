onload = () => {
  document.getElementById("button").onclick = (event) => {
    let textarea = document.getElementById("textarea");
    let urls = textarea.value.split('\n');
    let index = 0;
    for (let url of urls) {
      if (url.indexOf("http") == -1) {
        return;
      }
      setTimeout((index) => {
        let width = 320;
        let height = 320;
        let left = (index % 5) * width;
        let top = (Math.floor(index / 5)) * height;
        sub_window = window.open(url, Math.random(), "width=" + width + ",height=" + height + ",top=" + top +  ",left=" + left);
        console.log(url);
        setTimeout(() => {
          sub_window.focus();
        }, 4000);
      }, (index + 1) * 2000, index);
      index++;
    }
  }
}
